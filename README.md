# Capteur température / humidité extérieure

Capteur Arduino (DHT11) qui envoie la température et l'humidité extérieures au hub par XBee (Zigbee, mode transparent), avec un affichage sur écran LCD I2C 16x2.

- Le **hub** envoie une requête de lecture, le capteur répond avec une trame de 4 octets.
- Le capteur envoie aussi ses mesures **tout seul** toutes les 10 s (désactivable).
- L'écran affiche `T:xx°C  H:xx%` et une barre de progression avant chaque rafraîchissement.

## Arborescence

```
capteur_temp_humid/        croquis Arduino (à ouvrir avec l'IDE)
  capteur_temp_humid.ino   setup() et loop()
  config.h                 TOUTES les valeurs à modifier (voir plus bas)
  liaison_xbee.h/.cpp      liaison XBee et réglage de sa destination
  mesures.h/.cpp           lecture du DHT11
  protocole.h/.cpp         trames avec le hub (bibliothèque FrameProtocol)
  lcd.h/.cpp               écran LCD
libraries/                 sous-module git : FrameProtocol, XBeeAT, EcostoreNodes
```

## Installation

1. Cloner le dépôt **avec** le sous-module (sinon `libraries/` reste vide) :
   ```
   git clone --recurse-submodules https://github.com/EcostoreProject/temp-hum-ext.git
   ```
   Si le dossier `libraries/` est vide : `git submodule update --init`.
2. Dans l'IDE Arduino : **Fichier > Préférences**, régler **l'emplacement du carnet de croquis** sur la racine du dépôt, puis redémarrer l'IDE. Les bibliothèques `FrameProtocol` et `XBeeAT` sont alors trouvées dans `libraries/`.
3. Installer depuis le **Gestionnaire de bibliothèques** :
   - **DHT sensor library** (Adafruit), avec **Adafruit Unified Sensor**
   - **LiquidCrystal I2C**
4. Ouvrir `capteur_temp_humid/capteur_temp_humid.ino`, choisir la carte et le port, puis téléverser.

## Câblage (broches par défaut)

| Élément | Branchement |
|---|---|
| XBee DOUT | broche 2 de l'Arduino (RX) |
| XBee DIN | broche 3 de l'Arduino (TX) |
| DHT11 DATA | broche 4 |
| LCD I2C | SDA et SCL de la carte (A4 et A5 sur une Uno) |

Si vous changez de broche, modifiez `PIN_XBEE_RX`, `PIN_XBEE_TX` ou `PIN_DHT` dans `config.h`.

## Valeurs à modifier : `capteur_temp_humid/config.h`

Ouvrir `config.h`, changer la valeur, puis téléverser de nouveau.

| Constante | Valeur actuelle | Quand la changer |
|---|---|---|
| `DEST_SH` | `0x0013A200` | Adresse du **XBee du hub** (partie haute, 8 chiffres hexa). Presque toujours `0013A200` pour un XBee. |
| `DEST_SL` | `0x40D8C1F5` | Adresse du **XBee du hub** (partie basse). **À changer si vous changez de hub.** |
| `DESTINATION_BROADCAST` | `false` | Mettre `true` pour un test sans connaître l'adresse du hub : le capteur envoie à tous les XBee. Remettre `false` ensuite. |
| `PERIODE_ENVOI_MS` | `10000` | Délai en ms entre deux envois sans requête. `0` = le capteur ne fait que répondre au hub. Garder au moins `2000` (limite du DHT11). |
| `LCD_ADRESSE` | `0x27` | Si l'écran reste vide ou sans texte, essayer `0x3F`. |
| `LCD_PERIODE_MS` | `5000` | Durée de la barre de progression, c'est aussi le délai entre deux rafraîchissements de l'écran. |
| `LCD_COLONNES`, `LCD_LIGNES` | `16`, `2` | Seulement si l'écran n'est pas un 16x2. |
| `ID_HUB`, `ID_TEMP`, `ID_HUMID` | `0b000`, `0b101`, `0b110` | Identifiants sur 3 bits dans les trames. |
| `VAL_ERREUR` | `0xFF` | Valeur envoyée quand le capteur est en erreur. |
| `BAUD_USB`, `BAUD_XBEE` | `9600` | Le débit du XBee doit être le même que `BAUD_XBEE`. |

### Où trouver l'adresse du XBee du hub (`DEST_SH` et `DEST_SL`)

L'adresse est celle du XBee branché sur le **hub**, pas celle du capteur.

- **Sur le module** : le numéro de série à 16 chiffres hexa est imprimé sur l'étiquette. Les 8 premiers chiffres sont `DEST_SH`, les 8 derniers `DEST_SL`.
- **Dans XCTU** : connecter le XBee, lire les paramètres **SH** et **SL**.

Exemple : numéro `0013A20040D8C1F5` donne `DEST_SH = 0x0013A200` et `DEST_SL = 0x40D8C1F5`.

## Réglage des XBee (XCTU)

À faire une fois sur **chaque** module (la destination du capteur est réglée par le code au démarrage) :

| Paramètre | Valeur |
|---|---|
| `CH` (canal) | `C` |
| `ID` (PAN ID) | `1234` |
| `BD` (débit) | `3` (9600 bauds) |
| Mode | transparent |

## Format de la trame (4 octets)

| Octet | Contenu |
|---|---|
| 1 | `0xAA` (synchronisation) |
| 2 | `dest (3 bits) \| source (3 bits) \| commande (2 bits)` |
| 3 | valeur sur 8 bits non signés |
| 4 | somme des 3 premiers octets, modulo 256 |

Commande : `00` = lecture (requête du hub), `01` = écriture (réponse du capteur), `11` = erreur.

Valeurs envoyées :
- **Température** : entier en °C, de 0 à 50 (limite du DHT11).
- **Humidité** : entier en %, de 0 à 100.
- **Erreur** : commande `11` et valeur `0xFF`.

Exemples de réponses du capteur au hub (`ID_TEMP = 101`, `ID_HUMID = 110`) :

| Cas | Trame |
|---|---|
| 23 °C | `AA 15 17 D6` |
| 60 % d'humidité | `AA 19 3C FF` |
| Erreur de lecture de la température | `AA 17 FF C0` |

## Tester

1. Ouvrir le moniteur série à **9600** bauds. Au démarrage on doit voir `XBee configure` puis `Capteur pret`.
2. Chaque trame envoyée s'affiche : `TX : AA 15 17 D6`.
3. L'écran doit afficher les mesures après la première barre de progression.

## Problèmes courants

| Symptôme | Piste |
|---|---|
| `Mode commande impossible` | Le XBee ne répond pas : vérifier l'alimentation, que DOUT/DIN sont croisés avec les broches 2 et 3, et que le débit du XBee est bien 9600 (`BD = 3`). |
| Le hub ne reçoit rien | Vérifier `DEST_SH` / `DEST_SL`, ou tester avec `DESTINATION_BROADCAST = true`. Vérifier aussi `CH` et `ID` (PAN ID) sur les deux XBee. |
| Écran vide | Essayer `LCD_ADRESSE = 0x3F`, ou tourner le potentiomètre de contraste au dos de l'écran. |
| `Erreur capteur` sur l'écran | Vérifier le branchement du DHT11 (une résistance de 10 kΩ entre DATA et VCC pour le modèle à 4 broches). Le DHT11 mesure de 0 à 50 °C seulement. |
| Erreur de compilation `frame_protocol.h` ou `xbee_at.h` introuvable | Le sous-module n'est pas chargé ou l'emplacement du carnet de croquis n'est pas la racine du dépôt (voir Installation). |
