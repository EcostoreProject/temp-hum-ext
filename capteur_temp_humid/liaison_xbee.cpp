#include "liaison_xbee.h"
#include "config.h"
#include <xbee_at.h>

SoftwareSerial xbee(PIN_XBEE_RX, PIN_XBEE_TX);

// Ecrire sur l'UART
static void uartWrite(const uint8_t *data, uint16_t len) {
  xbee.write(data, len);
}

// Attend de recevoir OK avant la fin du délai
static bool attendreChaine(const char *attendu, uint32_t timeout_ms) {
  uint8_t n = strlen(attendu);
  uint8_t i = 0;
  uint32_t debut = millis();

  while (millis() - debut < timeout_ms) {
    if (xbee.available()) {
      char c = xbee.read();
      if (c == attendu[i]) {
        if (++i == n) return true;
      } else {
        i = (c == attendu[0]) ? 1 : 0;
      }
    }
  }
  return false;
}

static void attendreMs(uint32_t ms) {
  delay(ms);
}

// Envoie une commande AT sans valeur (ex. "ATAC") et attend OK
static bool envoyerAT(const char *cmd) {
  xbee.print(cmd);
  xbee.print('\r');
  return attendreChaine("OK", 1000);
}

// Envoie une commande AT avec une valeur hexadécimale (ex. "ATMY", 0x0002)
static bool envoyerATHex(const char *cmd, uint16_t valeur) {
  xbee.print(cmd);
  xbee.print(valeur, HEX);
  xbee.print('\r');
  return attendreChaine("OK", 1000);
}

static uint16_t myCourant = 0;

// Règle le MY du capteur puis la destination :
static bool reglerAdresses() {
  if (!envoyerATHex("ATMY", MY_TEMP)) return false;
  myCourant = MY_TEMP;

  if (DESTINATION_BROADCAST) {
    return xbee_set_destination_broadcast();  // applique aussi ATMY (ATAC)
  }
  bool ok = envoyerATHex("ATDH", 0)
            && envoyerATHex("ATDL", DEST_MY)
            && envoyerAT("ATAC");
  return ok;
}

// Initialisation
void liaisonXbeeInit() {
  xbee.begin(BAUD_XBEE);

  xbee_driver_t xbeedriver = { uartWrite, attendreChaine, attendreMs };
  xbee_init(&xbeedriver);

  if (xbee_enter_command_mode()) {
    bool ok = reglerAdresses();
    xbee_exit_command_mode();
    Serial.println(ok ? F("XBee configure") : F("Reglage MY/DL echoue"));
  } else {
    Serial.println(F("Mode commande impossible"));
  }
}

// Change MY : mode commande, ATMY, ATAC, sortie. Bloquant environ 2,5 s
// (le XBee impose 1,1 s de silence avant et après "+++").
bool liaisonDefinirMY(uint16_t my) {
  if (my == myCourant) return true;
  if (!xbee_enter_command_mode()) return false;
  bool ok = envoyerATHex("ATMY", my) && envoyerAT("ATAC");
  xbee_exit_command_mode();
  if (ok) myCourant = my;
  return ok;
}
