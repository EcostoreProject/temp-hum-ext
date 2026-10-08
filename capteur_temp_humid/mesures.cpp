#include "mesures.h"
#include "config.h"
#include <DHT.h>

static DHT dht(PIN_DHT, DHT11);

void mesuresInit() {
  dht.begin();
}

bool mesurerTemp(uint8_t &valeur) {
  float t = dht.readTemperature();
  if (isnan(t) || t < 0 || t > 50) return false;
  valeur = (uint8_t)round(t);
  Serial.print("TEMP :");
  Serial.println(valeur);

  return true;
}

bool mesurerHumid(uint8_t &valeur) {
  float h = dht.readHumidity();
  if (isnan(h) || h < 0 || h > 100) return false;
  valeur = (uint8_t)round(h);
  Serial.print("HUMID :");
  Serial.println(valeur);
  return true;
}
