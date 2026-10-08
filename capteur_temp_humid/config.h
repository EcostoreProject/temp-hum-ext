#pragma once

#include <Arduino.h>

// Broches
#define PIN_XBEE_RX  2
#define PIN_XBEE_TX  3
#define PIN_DHT      4

// Bauds
#define BAUD_USB     9600
#define BAUD_XBEE    9600

// Identifiants
const uint8_t ID_HUB   = 0b000;
const uint8_t ID_TEMP  = 0b101;
const uint8_t ID_HUMID = 0b110;

// Table des valeurs d'erreurs
const uint8_t VAL_ERREUR = 0xFF;

// Destination
const uint32_t DEST_SH = 0x0013A200;
const uint32_t DEST_SL = 0x40D8C1F5;
const bool DESTINATION_BROADCAST = false;

// Délais d'envoi sans requète
const unsigned long PERIODE_ENVOI_MS = 10000;

// Ecran LCD I2C
const uint8_t LCD_ADRESSE  = 0x27;
const uint8_t LCD_COLONNES = 16;
const uint8_t LCD_LIGNES   = 2;
const unsigned long LCD_PERIODE_MS = 5000;   // durée de la barre = délai entre deux rafraîchissements
