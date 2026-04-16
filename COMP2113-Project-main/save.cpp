/*
 * save.cpp
 * Role 4: Save / Load System
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey — The Yokohama Quest
 *
 * Saves and loads the full game state to/from save.txt.
 * Format: line-by-line values in a fixed order.
 */

#include "save.h"
#include "pet.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

static const char* SAVE_FILE = "save.txt";
static const string SAVE_HEADER = "ABYSSAL_ODYSSEY_SAVE";

// ============================================================================
// SAVE SYSTEM NAMESPACE
// ============================================================================

namespace SaveSystem {

bool saveGame(const Game& game, const SanityFatigue& sf) {
    ofstream out(SAVE_FILE);
    if (!out.is_open()) {
        cout << "  [!] Failed to open save file for writing." << endl;
        return false;
    }

    const GameState& s = game.getGameState();

    out << SAVE_HEADER << endl;

    int diffInt = 0;
    switch (s.difficulty) {
        case Difficulty::STORY:  diffInt = 0; break;
        case Difficulty::EASY:   diffInt = 1; break;
        case Difficulty::MEDIUM: diffInt = 2; break;
        case Difficulty::HARD:   diffInt = 3; break;
    }
    out << diffInt << endl;
    out << (s.pet != nullptr ? 1 : 0) << endl;

    out << s.currentDay << endl;
    out << s.ship.distance << endl;
    out << s.ship.durability << endl;
    out << s.crew.stamina << endl;
    out << s.crew.sanity << endl;
    out << s.resources.food << endl;
    out << s.resources.freshWater << endl;
    out << s.resources.gold << endl;
    out << s.consecutiveSailingDays << endl;
    out << s.daysWithoutWaterResupply << endl;

    if (s.pet != nullptr) {
        out << s.pet->petFood << endl;
        out << s.pet->petMood << endl;
    } else {
        out << 0 << endl;
        out << 0 << endl;
    }

    out << g_pet.name << endl;
    int typeInt = 0;
    switch (g_pet.type) {
        case PetType::CAT:    typeInt = 0; break;
        case PetType::DOG:    typeInt = 1; break;
        case PetType::PARROT: typeInt = 2; break;
    }
    out << typeInt << endl;
    out << (g_pet.alive ? 1 : 0) << endl;
    out << g_pet.daysAsCompanion << endl;
    out << (g_pet.bffUnlocked ? 1 : 0) << endl;
    out << (g_pet.hasPet ? 1 : 0) << endl;

    out << sf.consecutiveSailingDays << endl;
    out << sf.extraBadEventChance << endl;

    out.close();
    cout << "  Game saved successfully!" << endl;
    return true;
}

bool hasSaveFile() {
    ifstream in(SAVE_FILE);
    if (!in.is_open()) return false;

    string header;
    getline(in, header);
    in.close();
    return header == SAVE_HEADER;
}

bool peekSave(Difficulty& diff, bool& hasPet) {
    ifstream in(SAVE_FILE);
    if (!in.is_open()) return false;

    string header;
    getline(in, header);
    if (header != SAVE_HEADER) {
        in.close();
        return false;
    }

    int diffInt, petInt;
    in >> diffInt >> petInt;
    in.close();

    switch (diffInt) {
        case 0: diff = Difficulty::STORY;  break;
        case 1: diff = Difficulty::EASY;   break;
        case 2: diff = Difficulty::MEDIUM; break;
        case 3: diff = Difficulty::HARD;   break;
        default: diff = Difficulty::EASY;  break;
    }
    hasPet = (petInt != 0);
    return true;
}

bool loadGame(Game& game, SanityFatigue& sf) {
    ifstream in(SAVE_FILE);
    if (!in.is_open()) {
        cout << "  [!] No save file found." << endl;
        return false;
    }

    string header;
    getline(in, header);
    if (header != SAVE_HEADER) {
        cout << "  [!] Save file is corrupted." << endl;
        in.close();
        return false;
    }

    int diffInt, petInt;
    in >> diffInt >> petInt;

    GameState& s = game.getMutableState();

    in >> s.currentDay;
    in >> s.ship.distance;
    in >> s.ship.durability;
    in >> s.crew.stamina;
    in >> s.crew.sanity;
    in >> s.resources.food;
    in >> s.resources.freshWater;
    in >> s.resources.gold;
    in >> s.consecutiveSailingDays;
    in >> s.daysWithoutWaterResupply;

    int petFood, petMood;
    in >> petFood >> petMood;
    if (s.pet != nullptr) {
        s.pet->petFood = petFood;
        s.pet->petMood = petMood;
    }

    in.ignore();
    getline(in, g_pet.name);

    int typeInt, aliveInt, bffInt, hasPetInt;
    in >> typeInt >> aliveInt >> g_pet.daysAsCompanion >> bffInt >> hasPetInt;

    switch (typeInt) {
        case 0: g_pet.type = PetType::CAT;    break;
        case 1: g_pet.type = PetType::DOG;    break;
        case 2: g_pet.type = PetType::PARROT; break;
        default: g_pet.type = PetType::CAT;   break;
    }
    g_pet.alive = (aliveInt != 0);
    g_pet.bffUnlocked = (bffInt != 0);
    g_pet.hasPet = (hasPetInt != 0);

    in >> sf.consecutiveSailingDays;
    in >> sf.extraBadEventChance;

    in.close();
    cout << "  Game loaded successfully!" << endl;
    return true;
}

} // namespace SaveSystem
