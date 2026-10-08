#pragma once

#include <Arduino.h>

void mesuresInit();

bool mesurerTemp(uint8_t &valeur);

bool mesurerHumid(uint8_t &valeur);
