#include "liaison_xbee.h"
#include "config.h"
#include <xbee_at.h>

SoftwareSerial xbee(PIN_XBEE_RX, PIN_XBEE_TX);

// Ecrire sur l'UART
static void uartWrite(const uint8_t *data, uint16_t len) {
  xbee.write(data,len);
}

// Attend de recevoir OK avant la fin du délai
static bool attendreChaine(const char *attendu, uint32_t timeout_ms) {
  uint8_t n = strlen(attendu);
  uint8_t i = 0;
  uint32_t debut = millis();

  while (millis()-debut < timeout_ms) {
    if (xbee.available()) {
      char c =xbee.read();
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

// Initialisation
void liaisonXbeeInit() {
  xbee.begin(BAUD_XBEE);

  xbee_driver_t xbeedriver = { uartWrite, attendreChaine, attendreMs };
  xbee_init(&xbeedriver);

  if (xbee_enter_command_mode()) {
    if (DESTINATION_BROADCAST) xbee_set_destination_broadcast();
    else xbee_set_destination_64(DEST_SH, DEST_SL);
    xbee_exit_command_mode();
    Serial.println(F("XBee configure"));
  } else {
    Serial.println(F("Mode commande impossible"));
  }
}
