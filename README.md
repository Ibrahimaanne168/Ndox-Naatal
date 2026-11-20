# NDox et Naatal — plateforme IoT agricole (version 1)

## Démarrer sous Windows / VS Code
```powershell
py -m venv .venv
.venv\Scripts\activate
pip install -r requirements.txt
python app.py
```
Ouvrir http://127.0.0.1:5000

Démo accélérée (assèchement ×5) : `$env:NDOX_SIM_SPEED=5; python app.py`

## Fonctionnalités de cette version
1. **Cultures** : tomate, oignon, riz, mil, laitue ; phases pépinière, croissance, floraison, fructification, récolte. Les seuils d'humidité min/max changent avec la culture et la phase (`backend/crops.py`).
2. **Eau** : le sol simulé s'assèche progressivement ; détection automatique du sol sec (humidité < seuil min) ; notification d'arrosage ; boutons Démarrer / Arrêter la pompe. La pompe fait remonter l'humidité simulée et s'arrête automatiquement (sécurité simulée) au seuil max.
3. **Engrais** : pH, salinité, N, P, K comparés aux plages de la culture ; notification quand un apport mérite d'être vérifié ; conseils selon la culture et la phase.
4. **Interface** : tableau de bord, graphique d'évolution (sol, température, air, azote, avec seuils et marques de pompe), historique des mesures, journal des actions, centre de notifications, badges RÉEL / SIMULÉ partout.

**Simulé dans cette version : la pompe et les notifications** (affichées dans l'application, marquées « simulée »). Aucune commande physique n'est envoyée.

## Modes
- Mesure ESP32 reçue < 35 s : **hybride** (sol, température et humidité de l'air réels ; pH, salinité, NPK, pluie, pompe simulés).
- Sinon : **simulation complète**.

## API
| Route | Rôle |
|---|---|
| `GET /api/dashboard` | état complet (valeurs, sources, seuils, nutriments, pompe, conseils) |
| `POST /api/settings` | `{"crop":"tomate","phase":"floraison"}` |
| `POST /api/pump` | `{"action":"start"}` ou `{"action":"stop"}` |
| `GET /api/notifications` · `POST /api/notifications/read` | centre de notifications |
| `GET /api/history?limit=50` | mesures + journal des actions |
| `POST /api/simulation/reset` | remet le sol simulé à 65 % |
| `POST /api/esp32` | réception des mesures ESP32 (la réponse contient déjà `pump_command`, préparation de la future commande réelle) |

Test sans capteur (fausse transmission logicielle, pas une mesure physique) :
```powershell
$body = @{parcel="Parcelle de Miss";soil_moisture=31.5;air_temperature=29.1;air_humidity=67} | ConvertTo-Json
Invoke-RestMethod -Uri http://127.0.0.1:5000/api/esp32 -Method Post -ContentType "application/json" -Body $body
```

## ESP32
Installer Arduino IDE et les bibliothèques `DHT sensor library` et `Adafruit Unified Sensor`. Dans `esp32/ndox_naatal_esp32.ino`, modifier le Wi-Fi et l'adresse IP du PC dans `SERVER_URL` (même réseau). Branchements de départ : capteur capacitif VCC 3,3 V, GND, AOUT GPIO34 ; DHT11 VCC 3,3 V, GND, DATA GPIO4 (pull-up pour un DHT11 nu). Ne jamais appliquer 5 V à une entrée ESP32. Calibrer `RAW_DRY` et `RAW_WET`.

## Étapes suivantes prévues
- Connexion physique de la pompe (relais sur l'ESP32, lecture de `pump_command`).
- Vrai système de notification (SMS / WhatsApp / e-mail).

## Limites
Les seuils par culture/phase et les plages de nutriments sont **indicatifs** : à valider avec un agronome et des analyses de sol locales. Le DHT11 mesure l'air, pas le sol ; le capteur capacitif doit être calibré. pH, salinité, NPK et pluie sont simulés. L'API est destinée à un réseau local de démonstration, sans authentification.
