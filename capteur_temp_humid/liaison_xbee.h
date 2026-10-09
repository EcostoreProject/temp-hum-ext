#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

extern SoftwareSerial xbee;

void liaisonXbeeInit();

// Change le MY du capteur : 0x1005 pour la température, 0x1006 pour l'humidité
// (ne fait rien si c'est déjà le bon)
bool liaisonDefinirMY(uint16_t my);
