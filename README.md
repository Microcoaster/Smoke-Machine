# 🎢 MicroCoaster - Module Smoke Machine (ESP32)

> Module de fumée pour montagnes russes miniatures avec contrôle WebSocket et logique simple par cycles (READY → SMOKING), sans cooldown local.

[![Version](https://img.shields.io/badge/version-0.1.0--alpha-blue.svg)](#)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-compatible-orange.svg)](https://platformio.org/)
[![ESP32](https://img.shields.io/badge/ESP32-compatible-green.svg)](https://www.espressif.com/)

---

## 📋 Description du Projet

### À quoi sert ce projet ?
Ce module pilote une **machine à fumée miniature** (résistance + ventilateur intégrés) à l’aide d’un **ESP32** et d’un **MOSFET**.  
La logique est **simple et sûre** : on déclenche la fumée pendant une **durée définie**, puis le module **revient en READY**.  
> ⚠️ Aucun *cooldown* n’est géré localement : si un pacing est requis, il est imposé par le **contrôleur maître**.

### Fonctionnalités principales
- ✅ Pilotage **MOSFET** sur **GPIO18 (D18)**
- ✅ États exposés : **READY** / **SMOKING**
- ✅ **WebSocket JSON** (contrôle distant) + **CLI série**
- ✅ Intégration cohérente avec l’écosystème **MicroCoaster**

---

## 🔧 Composants Hardware

### **ESP32 DevKit**
- Microcontrôleur principal (Arduino/PlatformIO)
- Gère la logique et la communication

### **Chaîne Puissance**
- **MOSFET N canal** → commande on/off de la machine à fumée (gate sur D18)
- **Alim** : 2× **18650** en série (≈7.4 V)
- **2 Step-Down** :
  - 7.4 V → **6 V** pour la smoke machine
  - 7.4 V → **5 V** pour l’ESP32

### **Machine à Fumée**
- Henglong 1/16 RC Tank (module fumée)
- Liquide compatible “Smoke One – Recharge”
- Liens (référence) :
  - https://fr.aliexpress.com/item/4000797962625.html
  - https://fr.aliexpress.com/item/1005008515819144.html

**Notes**
- **GND commun** obligatoire entre ESP32, MOSFET et alimentations.
- Ne pas activer la résistance sans ventilation fonctionnelle de la machine.

---

## 🧰 Stack Technique

- **Microcontrôleur** : ESP32
- **Langage** : C++ (Arduino)
- **Build** : PlatformIO (VS Code)
- **Communication** : WebSocket JSON + Port série (CLI)

---

## 📦 Installation

### 1) Cloner le dépôt
```bash
git clone https://github.com/MicroCoaster/Smoke-Machine.git
cd Smoke-Machine
````

### 2) Ouvrir dans VS Code

* Installer l’extension **PlatformIO**
* Ouvrir le dossier du projet

### 3) Choisir la carte

* Ouvrir `platformio.ini`
* Régler la carte (ex. `esp32dev`, `esp32-s3`, etc.)

### 4) Compiler & téléverser

```bash
pio run --target upload
```

💡 Ouvrir ensuite le **Moniteur Série** pour vérifier les logs.

---

## ⚙️ Configuration

Dans `include/config.h`, le module pilote **uniquement** la machine via **un MOSFET** connecté à **D18 (GPIO18)**.
Aucune logique de cooldown locale — seule une **limite de sécurité** borne la durée max d’un cycle.

```cpp
#pragma once
// ===== Brochage =====
#define PIN_HEATER        18    // D18 (ESP32) -> Gate MOSFET (machine = résistance + ventilateur internes)

// ===== Sécurité =====
#define HEATER_MAX_ON_MS  15000 // Durée max autorisée par cycle (ms)
```

---

## 🚀 Usage

### 🔹 États (parité Switch Track)

* `READY` : prêt à déclencher un cycle.
* `SMOKING` : fumée en cours pendant la durée demandée.

> Fin de `SMOKING` → retour immédiat à `READY`.
> Pas de cooldown local : la cadence d’enchaînement est imposée par le système maître.

### 🔸 Commandes Série (CLI)

```
> status          # { state: READY|SMOKING, remaining_ms: N, max_ms: HEATER_MAX_ON_MS }
> setdur 10       # fixe la durée (secondes)
> smoke           # lance un cycle (si state=READY)
```

### 🔸 API WebSocket (JSON)

* Connexion : `ws://<ip-esp32>/ws`

**Requêtes**

```json
{ "cmd": "STATUS" }
{ "cmd": "SET_DURATION", "s": 10 }
{ "cmd": "SMOKE" }
```

**Réponses (exemples)**

```json
{ "ok": true, "state": "READY", "duration_s": 10 }
{ "ok": true, "state": "SMOKING", "remaining_ms": 9800 }
{ "ok": true, "state": "READY" }
```

---

## 📡 Schéma de Câblage (texte)

```
ESP32 DevKit           MOSFET (N)            Smoke Machine
────────────           ───────────           ──────────────
GPIO18 (D18) ───────── Gate
5V ESP32     ────────── (alimentation ESP32)
GND ─────────────────── Source ────────────── GND machine
6V (step-down) ───────── Drain ────────────── +6V machine

Alimentation :
- 2×18650 en série (~7.4 V)
- Step-down 7.4→6 V (machine)
- Step-down 7.4→5 V (ESP32)
```

<img width="1467" height="624" alt="image" src="https://github.com/user-attachments/assets/25fb8095-2236-4821-bd78-7aa42300cb0d" />


---

## 📁 Structure du Projet

```
Smoke-Machine/
├── src/
│   └── main.cpp              # Code principal
├── include/
│   └── config.h              # Brochage + limites
├── platformio.ini            # Configuration PlatformIO
├── README.md                 # Ce fichier
└── LICENSE                   # Licence MIT
```

---

## 🎮 Commandes Disponibles (récap)

| Commande     | Description              | Effet                                |
| ------------ | ------------------------ | ------------------------------------ |
| `status`     | État courant du module   | READY/SMOKING + remaining_ms         |
| `setdur <s>` | Fixe la durée d’un cycle | Définit la durée en secondes         |
| `smoke`      | Lance la fumée           | Passe en SMOKING puis retourne READY |

---

## 📊 Logs Série (exemples)

```
[SMOKE] Boot OK
[SMOKE] State: READY
[SMOKE] Duration set to 10 s
[SMOKE] Command: smoke
[SMOKE] -> SMOKING (10000 ms)
[SMOKE] -> READY
```

---

## 🔒 Sécurité

* **HEATER_MAX_ON_MS** limite la durée d’un cycle (protection locale).
* Dimensionner le **MOSFET** selon le courant de la machine (et dissipation thermique).
* **GND commun** et câblage propre indispensables.

---

## 📚 Ressources

* ESP32 : [https://www.espressif.com/](https://www.espressif.com/)
* PlatformIO : [https://platformio.org/](https://platformio.org/)
* WebSockets Arduino : [https://github.com/Links2004/arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets)
* ArduinoJson : [https://arduinojson.org/](https://arduinojson.org/)

---

**🎢 Projet MicroCoaster – Smoke Machine (ESP32)**
*Construit avec ❤️ pour les passionnés de montagnes russes miniatures.*

```

Tu veux que je te le génère en **fichier `.md` téléchargeable** tout prêt à déposer dans ton repo ?
```
