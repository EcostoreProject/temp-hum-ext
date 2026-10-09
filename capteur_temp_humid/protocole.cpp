// protocole.cpp : utilise la bibliothèque FrameProtocol du dépôt Ecostore-Zigbee
#include "protocole.h"
#include "config.h"
#include "liaison_xbee.h"
#include "mesures.h"
#include <frame_protocol.h>

static FrameParser_t parser;

void protocoleInit() {
  frame_parser_init(&parser);
}

// Envoie une trame
static void envoyerReponse(uint8_t emet, FrameCmd_t cmd, uint8_t valeur) {
  FrameMsg_t msg;
  msg.dest_id = ID_HUB;
  msg.src_id  = emet;
  msg.cmd     = cmd;
  msg.value   = valeur;

  uint8_t trame[FRAME_TOTAL_SIZE];
  frame_pack(&msg, trame);
  xbee.write(trame, FRAME_TOTAL_SIZE);

  Serial.print(F("TX : "));
  for (uint8_t i = 0; i < FRAME_TOTAL_SIZE; i++) {
    Serial.print(trame[i], HEX);
    Serial.print(' ');
  }
  Serial.println();

  if (!DESTINATION_BROADCAST) liaisonDefinirMY(emet==ID_TEMP ? MY_HUMID : MY_TEMP);
}

// Envoyer une valeur
void envoyerMesure(uint8_t id) {
  uint8_t valeur;
  bool ok;

  if (id == ID_TEMP)        ok = mesurerTemp(valeur);
  else if (id == ID_HUMID)  ok = mesurerHumid(valeur);
  else return;              // ID inconnu

  // La mesure part avec le MY qui correspond à son type (1005 température, 1006 humidité)
  liaisonDefinirMY(id == ID_TEMP ? MY_TEMP : MY_HUMID);

  if (ok) envoyerReponse(id, FRAME_CMD_WRITE, valeur);
  else    envoyerReponse(id, FRAME_CMD_ERROR, VAL_ERREUR);
}

// Envoi sans requête
void envoiSpontane() {
  static unsigned long dernierEnvoi = 0;

  if (PERIODE_ENVOI_MS == 0) return;   // désactivé
  if (millis() - dernierEnvoi >= PERIODE_ENVOI_MS) {
    dernierEnvoi = millis();
    envoyerMesure(ID_TEMP);
    envoyerMesure(ID_HUMID);
  }
}

// Répond à une requête de lecture
static void traiterRequete(const FrameMsg_t &req) {
  envoyerMesure(req.dest_id);   // dest_id = l'ID du capteur interrogé
}

// Lit une requète
void lireRequete() {
  while (xbee.available()) {
    uint8_t c = xbee.read();
    FrameMsg_t msg;
    if (frame_parse_byte(&parser, c, &msg)) {
      if (msg.src_id == ID_HUB && msg.cmd == FRAME_CMD_READ) {
        traiterRequete(msg);
      }
    }
  }
}