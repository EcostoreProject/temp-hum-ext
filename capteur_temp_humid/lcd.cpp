// lcd.cpp : écran LCD I2C 16x2 (bibliothèque LiquidCrystal_I2C)
#include "lcd.h"
#include "config.h"
#include "mesures.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(LCD_ADRESSE, LCD_COLONNES, LCD_LIGNES);

static unsigned long debutCycle = 0;
static int dernierPas = -1;

// Crée 5 caractères : une barre remplie sur 1 à 5 colonnes de pixels
static void creerBarres() {
  for (byte i = 1; i <= 5; i++) {
    byte motif[8];
    byte ligne = (0x1F << (5 - i)) & 0x1F;
    for (byte j = 0; j < 8; j++) motif[j] = ligne;
    lcd.createChar(i, motif);
  }
}

// Dessine la barre de progression sur la 2e ligne (5 pas par colonne)
static void dessinerBarre(int pas) {
  lcd.setCursor(0, 1);
  for (int c = 0; c < LCD_COLONNES; c++) {
    int reste = pas - c * 5;
    if (reste >= 5)     lcd.write((byte)5);
    else if (reste > 0) lcd.write((byte)reste);
    else                lcd.write(' ');
  }
}

// Affiche les mesures sur la 1re ligne
static void afficherMesuresLCD(uint8_t temp, uint8_t humid, bool err) {
  lcd.setCursor(0, 0);

  if (err) {
    lcd.print("Erreur capteur ");
  } else {
    lcd.print("T:");
    lcd.print(temp);
    lcd.print((char)223);
    lcd.print("C  H:");
    lcd.print(humid);
    lcd.print("%   ");
  }
}

void lcdInit() {
  lcd.init();
  creerBarres();
  lcd.clear();
  lcd.backlight();
  debutCycle = millis();
  dernierPas = -1;
}

// Version non bloquante : la barre se remplit pendant LCD_PERIODE_MS,
// puis les mesures sont rafraîchies et le cycle recommence.
// loop() peut donc continuer à lire les requêtes du hub entre-temps.
void lcdMettreAJour() {
  const int total = LCD_COLONNES * 5;
  unsigned long ecoule = millis() - debutCycle;

  if (ecoule >= LCD_PERIODE_MS) {
    dessinerBarre(total);

    uint8_t temp = 0, humid = 0;
    bool ok = mesurerTemp(temp) && mesurerHumid(humid);
    afficherMesuresLCD(temp, humid, !ok);

    debutCycle = millis();
    dernierPas = -1;
    return;
  }

  int pas = (int)(ecoule * total / LCD_PERIODE_MS);
  if (pas != dernierPas) {
    dessinerBarre(pas);
    dernierPas = pas;
  }
}
