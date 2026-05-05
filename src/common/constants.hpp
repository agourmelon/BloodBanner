#pragma once

#include <array>
#include <string_view>
#include <utility>

// ─────────────────────────────────────────────
// Victoire
// ─────────────────────────────────────────────

// Nombre de châteaux + forteresses à contrôler pour gagner
inline constexpr int VICTORY_THRESHOLD = 10;

// ─────────────────────────────────────────────
// Joueurs
// ─────────────────────────────────────────────

inline constexpr int MIN_PLAYERS = 2;
inline constexpr int MAX_PLAYERS = 6;

// ─────────────────────────────────────────────
// Cartes
// ─────────────────────────────────────────────

inline constexpr int HOUSE_CARDS_PER_PLAYER = 7;

// ─────────────────────────────────────────────
// Unités de départ
// ─────────────────────────────────────────────

// Unités placées sur la forteresse de départ
inline constexpr int STARTING_FOOTMEN_ON_STRONGHOLD = 1;
inline constexpr int STARTING_KNIGHTS_ON_STRONGHOLD = 1;

// ─────────────────────────────────────────────
// Génération de carte
// ─────────────────────────────────────────────

// Nombre de provinces = MAP_BASE_PROVINCES + MAP_PROVINCES_PER_PLAYER * nb_joueurs
// Augmenter MAP_BASE_PROVINCES pour des cartes plus denses
inline constexpr int MAP_BASE_PROVINCES       = 8;
inline constexpr int MAP_PROVINCES_PER_PLAYER = 4;

// Proportion de provinces avec une structure parmi toutes les provinces
// ex: 0.3 = 30% des provinces ont un château ou une forteresse
inline constexpr float MAP_STRUCTURE_RATIO = 0.3f;

// Parmi les structures, proportion de châteaux vs forteresses
// ex: 0.7 = 70% châteaux, 30% forteresses
inline constexpr float MAP_CASTLE_RATIO = 0.7f;

// ─────────────────────────────────────────────
// Forteresses de départ par maison
// ─────────────────────────────────────────────

// Clef : nom de la maison — Valeur : nom de la forteresse de départ
// Ajouter une maison = ajouter une ligne ici
inline constexpr std::array<std::pair<std::string_view, std::string_view>, 6> HOUSE_STRONGHOLDS = {{
    {"Barathéon", "Accalmie"},
    {"Stark", "Winterfell"},
    {"Lannister", "Castral Roc"},
    {"Greyjoy", "Pyk"},
    {"Martell", "Lancehélion"},
    {"Tyrell", "Hautjardin"},
}};
