#include "config.h"
#include "liaison_xbee.h"
#include "mesures.h"
#include "protocole.h"
#include "lcd.h"

void setup() {
  Serial.begin(BAUD_USB);
  lcdInit();
  mesuresInit();
  protocoleInit();
  liaisonXbeeInit();
  Serial.println(F("Capteur pret"));
}

void loop() {
  lireRequete();
  envoiSpontane();
  lcdMettreAJour();
}
