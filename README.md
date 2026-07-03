# BloodBanner — CLAUDE.md

## Présentation

Implémentation en C++23 du jeu de société "Le Trône de Fer" (2e édition).
Nom du projet : **BloodBanner**.

---

## Règles du jeu

### Familles

6 familles jouables : Barathéon, Stark, Lannister, Greyjoy, Martell, Tyrell.

### Éléments de jeu

Chaque joueur dispose de :

- **7 cartes personnages**, utilisées lors des batailles
- **Des unités** de plusieurs types :
  - **Fantassin** : force de combat 1, coût de recrutement 1
  - **Chevalier** : force de combat 2, coût de recrutement 2 (ou 1 si conversion depuis un fantassin)
  - **Bateau** _(futur)_ : force 1, coût 1, se déplace uniquement en mer, recrutement uniquement dans les mers ou ports adjacents à un château/forteresse
  - **Engin de siège** _(futur)_ : force 4 contre château/forteresse, 0 sinon et en défense, détruit s'il doit battre en retraite

### Carte

- Divisée en **provinces**
- Les unités de 2 joueurs différents **ne peuvent pas cohabiter** sur une même province — un combat est déclenché
- Certaines provinces ont au plus **un château OU une forteresse** :
  - **Château** : 1 point de victoire + 1 point de recrutement
  - **Forteresse** : 1 point de victoire + 2 points de recrutement

### Mise en place

- La carte est **générée aléatoirement** au lancement, proportionnée au nombre de joueurs
- Chaque joueur commence avec des unités sur sa **forteresse de départ** et sur **2-3 provinces adjacentes**

Forteresses de départ :

| Famille   | Forteresse  |
| --------- | ----------- |
| Barathéon | Accalmie    |
| Stark     | Winterfell  |
| Lannister | Castral Roc |
| Greyjoy   | Pyk         |
| Martell   | Lancehélion |
| Tyrell    | Hautjardin  |

---

## Déroulement d'un tour

### Phase 1 — Planification des ordres

- **Simultanément et secrètement**, chaque joueur assigne un ordre à chaque province où il dispose d'au moins une unité
- **5 ordres possibles** :
  - **Marche** : déplace des unités vers une province adjacente
  - **Défense** : +1 force défensive (passif)
  - **Soutien** : renforce un combat dans une province adjacente (passif)
  - **Recrutement** : recrute de nouvelles unités (uniquement sur château/forteresse)
  - **Raid** : retire un ordre Soutien ou Recrutement adverse adjacent
- Une province sans unité ne peut pas recevoir d'ordre
- Quand tous les joueurs ont fini, on passe à la résolution

### Phase 2 — Résolution des ordres

Tous les ordres sont révélés. Résolution dans cet ordre, et à chaque fois dans l'ordre des joueurs :

#### 1. Raid

- Retire un ordre **Soutien** ou **Recrutement** adverse dans une province adjacente

#### 2. Marche

- Déplace des unités vers une province adjacente
- Si la destination contient des unités adverses → **combat**

#### 3. Recrutement

- Recrute de nouvelles unités sur une province avec château ou forteresse
- Les points de recrutement non consommés sont perdus :
  - 1 fantassin = 1 point
  - 1 chevalier = 2 points
  - Conversion fantassin → chevalier = 1 point

> Les ordres **Défense** et **Soutien** sont passifs — ils n'ont d'effet que lors d'un combat.
> Tout ordre résolu est retiré du plateau.

---

## Déroulement d'un combat

Déclenché lorsque des unités arrivent sur une province occupée par un adversaire.
Le joueur sur place est le **défenseur**, l'arrivant est l'**attaquant**.

### 1. Force de base

- 1 point par fantassin, 2 points par chevalier
- Les unités en retraite ont une force de 0
- Ordre de **Défense** sur la province : +1 pour le défenseur

### 2. Soutiens

- Province adjacente avec ordre de **Soutien** appartenant à un belligérant → force ajoutée à son camp
- Soutien d'un joueur tiers → ce joueur choisit librement quel camp soutenir, ou ne soutient personne

### 3. Cartes personnages

- Chaque belligérant choisit **secrètement** une carte personnage
- Révélation **simultanée** — la force de la carte s'ajoute à la force de combat

### 4. Résolution

- Le joueur avec la plus grande force **remporte la bataille**
- En cas d'égalité : le défenseur l'emporte

### 5. Retraite

- Le perdant retire ses unités dans une province adjacente sans unités ennemies
- Les unités en retraite ont une force de 0 jusqu'à la fin du tour
- Une unité déjà en retraite qui doit de nouveau battre en retraite est **détruite**

### 6. Épées et tours

- **Épées** (sur la carte du vainqueur) : le vaincu retire ce nombre d'unités parmi ses troupes en retraite
- **Tours** (sur la carte du vaincu) : chaque tour annule une épée adverse

### 7. Cartes jouées

- Une carte jouée est défaussée
- Si toutes les cartes d'un joueur sont défaussées → toutes reviennent en main

---

## Condition de victoire

La partie s'arrête **immédiatement** lorsqu'un joueur contrôle **10 châteaux et/ou forteresses**.

---

## Architecture générale

### Structure du projet

```
bloodbanner/
├── src/
│   ├── common/        — types de données partagés (header-only)
│   ├── server/        — logique serveur
│   └── client/        — logique client
├── tests/
│   ├── common/        — tests unitaires
│   └── integration/   — tests d'intégration
├── CMakeLists.txt
├── CLAUDE.md
└── CONTEXT.md
```

### Principes

- **`common/` ne contient que des types de données** — aucune logique de jeu, aucune validation
- **La validation des règles est déléguée aux couches supérieures** (serveur) — SRP
- **Les invariants structurels** restent dans les classes de base (ex: accès à une province inexistante)
- **Pas de namespace global** — décision explicite

### Patterns utilisés

- **`std::variant` + visiteur** pour les types polymorphiques extensibles :
  - `Unit      = std::variant<Footman, Knight>`
  - `Order     = std::variant<MarchOrder, DefenseOrder, SupportOrder, MusterOrder, RaidOrder>`
  - `Structure = std::variant<Castle, Stronghold>`
- **Variables membres statiques** pour les propriétés invariantes d'un type :
  ```cpp
  struct Footman : UnitBase {
      static constexpr std::string_view name            = "Fantassin";
      static constexpr int              recruitmentCost = 1;
      static constexpr int              combatStrength  = 1;
  };
  ```
- **Interface plugin** pour la topologie de carte (`IMapTopologyStrategy`)
- **`std::optional<std::reference_wrapper<T>>`** pour les références nullables

### Composants serveur (à implémenter)

- `map_generator` — génération procédurale de carte (grille hexagonale, coordonnées cubiques)
- `map_loader` — chargement de carte depuis un fichier JSON (nlohmann/json)
- `game_engine` — machine à états des phases de jeu
- `order_resolver` — résolution Raid / Marche / Recrutement (visitor pattern)
- `combat_resolver` — résolution des combats
- `victory_checker` — détection de la condition de victoire
- `network_server` — TCP, sessions joueurs

### Extensions futures anticipées

- **Mers et ports** : nouveau type de province pour les bateaux
- **Pions d'influence** : maintien du contrôle d'une province vide
- **Topologie Delaunay** : alternative à la grille hexagonale
- **Ordres spéciaux** : effets des cartes personnages
