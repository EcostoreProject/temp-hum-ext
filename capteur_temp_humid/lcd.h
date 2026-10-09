// lcd.h : affichage des mesures sur l'écran LCD I2C 16x2
#pragma once

#include <Arduino.h>

// À appeler une fois dans setup()
void lcdInit();

// À appeler à chaque tour de loop() : fait avancer la barre de progression
// et, toutes les LCD_PERIODE_MS, mesure et affiche température et humidité
void lcdMettreAJour();
