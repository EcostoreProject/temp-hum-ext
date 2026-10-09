// protocole.h : échange de trames avec le hub (bibliothèque FrameProtocol)
#pragma once

#include <Arduino.h>

void protocoleInit();

void lireRequete();

void envoyerMesure(uint8_t id);

void envoiSpontane();