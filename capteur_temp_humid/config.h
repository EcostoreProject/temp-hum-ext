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
// Adresses 16 bits (registre MY des XBee), en hexadécimal
const uint16_t DEST_MY  = 0x1000;   // MY du XBee du hub
const uint16_t MY_TEMP  = 0x1005;   // MY du capteur quand il envoie la température
const uint16_t MY_HUMID = 0x1006;   // MY du capteur quand il envoie l'humidité
const bool DESTINATION_BROADCAST = false;

// Délais d'envoi sans requète
const unsigned long PERIODE_ENVOI_MS = 10000;

// Ecran LCD I2C
const uint8_t LCD_ADRESSE  = 0x27;
const uint8_t LCD_COLONNES = 16;
const uint8_t LCD_LIGNES   = 2;
const unsigned long LCD_PERIODE_MS = 5000;   // durée de la barre = délai entre deux rafraîchissements
