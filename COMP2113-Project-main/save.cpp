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
#include <sstream>
#include <cstdio>

using namespace std;

static const char *SAVE_FILE = "save.txt";
static const string SAVE_HEADER = "ABYSSAL_ODYSSEY_SAVE_V2"; // Versioned for future updates
static const int SAVE_VERSION = 2;

// ============================================================================
// Helper Functions (Internal)
// ============================================================================

namespace
{

    // Convert Difficulty to/from int safely
    int difficultyToInt(Difficulty d)
    {
        switch (d)
        {
        case Difficulty::STORY:
            return 0;
        case Difficulty::EASY:
            return 1;
        case Difficulty::MEDIUM:
            return 2;
        case Difficulty::HARD:
            return 3;
        default:
            return 1; // Default to EASY
        }
    }

    Difficulty intToDifficulty(int val)
    {
        switch (val)
        {
        case 0:
            return Difficulty::STORY;
        case 1:
            return Difficulty::EASY;
        case 2:
            return Difficulty::MEDIUM;
        case 3:
            return Difficulty::HARD;
        default:
            return Difficulty::EASY;
        }
    }

    // Convert PetType to/from int safely
    int petTypeToInt(PetType t)
    {
        switch (t)
        {
        case PetType::CAT:
            return 0;
        case PetType::DOG:
            return 1;
        case PetType::PARROT:
            return 2;
        default:
            return 0;
        }
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
    out << s.daysWithoutFood << endl;

    if (s.pet != nullptr) {
        out << s.pet->petFood << endl;
        out << s.pet->petMood << endl;
    } else {
        out << 0 << endl;
        out << 0 << endl;
    }

    // Write a line to file with error checking
    bool writeLine(ofstream &out, const string &line)
    {
        if (!(out << line << endl))
        {
            cerr << "  [!] Write error: " << line << endl;
            return false;
        }
        return true;
    }

    // Read a line and parse value
    template <typename T>
    bool readValue(ifstream &in, T &value, const string &fieldName)
    {
        if (!(in >> value))
        {
            cerr << "  [!] Failed to read field: " << fieldName << endl;
            return false;
        }
        return true;
    }

} // anonymous namespace

// ============================================================================
// SAVE SYSTEM NAMESPACE
// ============================================================================

namespace SaveSystem
{

    bool saveGame(const Game &game, const SanityFatigue &sf)
    {
        ofstream out(SAVE_FILE);
        if (!out.is_open())
        {
            cout << "  [!] Failed to open save file for writing." << endl;
            return false;
        }

        const GameState &s = game.getGameState();

        // Write header with version
        if (!writeLine(out, SAVE_HEADER))
            return false;
        if (!writeLine(out, to_string(SAVE_VERSION)))
            return false;

        // Write difficulty and pet existence
        if (!writeLine(out, to_string(difficultyToInt(s.difficulty))))
            return false;
        if (!writeLine(out, to_string(s.pet != nullptr ? 1 : 0)))
            return false;

        // Write core game state (one per line for clarity)
        if (!writeLine(out, to_string(s.currentDay)))
            return false;
        if (!writeLine(out, to_string(s.ship.distance)))
            return false;
        if (!writeLine(out, to_string(s.ship.durability)))
            return false;
        if (!writeLine(out, to_string(s.crew.stamina)))
            return false;
        if (!writeLine(out, to_string(s.crew.sanity)))
            return false;
        if (!writeLine(out, to_string(s.resources.food)))
            return false;
        if (!writeLine(out, to_string(s.resources.freshWater)))
            return false;
        if (!writeLine(out, to_string(s.resources.gold)))
            return false;
        if (!writeLine(out, to_string(s.consecutiveSailingDays)))
            return false;
        if (!writeLine(out, to_string(s.daysWithoutWaterResupply)))
            return false;

        // Write pet data (if exists)
        if (s.pet != nullptr)
        {
            if (!writeLine(out, to_string(s.pet->petFood)))
                return false;
            if (!writeLine(out, to_string(s.pet->petMood)))
                return false;
        }
        else
        {
            if (!writeLine(out, "0"))
                return false;
            if (!writeLine(out, "0"))
                return false;
        }

        // Write global pet data
        if (!writeLine(out, g_pet.name))
            return false;
        if (!writeLine(out, to_string(petTypeToInt(g_pet.type))))
            return false;
        if (!writeLine(out, to_string(g_pet.alive ? 1 : 0)))
            return false;
        if (!writeLine(out, to_string(g_pet.daysAsCompanion)))
            return false;
        if (!writeLine(out, to_string(g_pet.bffUnlocked ? 1 : 0)))
            return false;
        if (!writeLine(out, to_string(g_pet.hasPet ? 1 : 0)))
            return false;

        // Write SanityFatigue data
        if (!writeLine(out, to_string(sf.consecutiveSailingDays)))
            return false;
        if (!writeLine(out, to_string(sf.extraBadEventChance)))
            return false;

        out.close();
        cout << "  [✓] Game saved successfully!" << endl;
        return true;
    }

    bool hasSaveFile()
    {
        ifstream in(SAVE_FILE);
        if (!in.is_open())
            return false;

        string header;
        getline(in, header);
        in.close();

        // Check for either old or new save format
        return (header == SAVE_HEADER || header == "ABYSSAL_ODYSSEY_SAVE");
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
    in >> s.daysWithoutFood;

    int petFood, petMood;
    in >> petFood >> petMood;
    if (s.pet != nullptr) {
        s.pet->petFood = petFood;
        s.pet->petMood = petMood;
    }

    bool loadGame(Game &game, SanityFatigue &sf)
    {
        ifstream in(SAVE_FILE);
        if (!in.is_open())
        {
            cout << "  [!] No save file found." << endl;
            return false;
        }

        string header;
        getline(in, header);
        if (header != SAVE_HEADER && header != "ABYSSAL_ODYSSEY_SAVE")
        {
            cout << "  [!] Save file is corrupted or incompatible." << endl;
            in.close();
            return false;
        }

        // Read version if using new format
        int saveVersion = 1;
        if (header == SAVE_HEADER)
        {
            string versionStr;
            getline(in, versionStr);
            saveVersion = stoi(versionStr);
        }

        int diffInt, petInt;
        if (!readValue(in, diffInt, "difficulty"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, petInt, "pet existence"))
        {
            in.close();
            return false;
        }

        GameState &s = game.getMutableState();

        // Read core game state
        if (!readValue(in, s.currentDay, "currentDay"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, s.ship.distance, "distance"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, s.ship.durability, "durability"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, s.crew.stamina, "stamina"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, s.crew.sanity, "sanity"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, s.resources.food, "food"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, s.resources.freshWater, "freshWater"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, s.resources.gold, "gold"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, s.consecutiveSailingDays, "consecutiveSailingDays"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, s.daysWithoutWaterResupply, "daysWithoutWaterResupply"))
        {
            in.close();
            return false;
        }

        // Read pet data
        int petFood, petMood;
        if (!readValue(in, petFood, "petFood"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, petMood, "petMood"))
        {
            in.close();
            return false;
        }

        if (s.pet != nullptr)
        {
            s.pet->petFood = petFood;
            s.pet->petMood = petMood;
        }

        // Skip newline before reading string
        in.ignore();

        // Read global pet data
        getline(in, g_pet.name);

        int typeInt, aliveInt, bffInt, hasPetInt;
        if (!readValue(in, typeInt, "petType"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, aliveInt, "petAlive"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, g_pet.daysAsCompanion, "daysAsCompanion"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, bffInt, "bffUnlocked"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, hasPetInt, "hasPet"))
        {
            in.close();
            return false;
        }

        g_pet.type = intToPetType(typeInt);
        g_pet.alive = (aliveInt != 0);
        g_pet.bffUnlocked = (bffInt != 0);
        g_pet.hasPet = (hasPetInt != 0);

        // Read SanityFatigue data
        if (!readValue(in, sf.consecutiveSailingDays, "sf.consecutiveSailingDays"))
        {
            in.close();
            return false;
        }
        if (!readValue(in, sf.extraBadEventChance, "sf.extraBadEventChance"))
        {
            in.close();
            return false;
        }

        in.close();
        cout << "  [✓] Game loaded successfully! (Save version: " << saveVersion << ")" << endl;
        return true;
    }

    // NEW: Utility function to delete save file (useful for new game or reset)
    bool deleteSaveFile()
    {
        if (remove(SAVE_FILE) == 0)
        {
            cout << "  [✓] Save file deleted." << endl;
            return true;
        }
        else
        {
            cout << "  [!] No save file to delete." << endl;
            return false;
        }
    }

    // NEW: Get save file info without full load
    SaveInfo getSaveInfo()
    {
        SaveInfo info;
        info.exists = false;
        info.hasPet = false;
        info.difficulty = Difficulty::EASY;
        info.dayCount = 0;

        ifstream in(SAVE_FILE);
        if (!in.is_open())
            return info;

        string header;
        getline(in, header);
        if (header != SAVE_HEADER && header != "ABYSSAL_ODYSSEY_SAVE")
        {
            in.close();
            return info;
        }

        info.exists = true;

        // Skip version if needed
        if (header == SAVE_HEADER)
        {
            string versionLine;
            getline(in, versionLine);
        }

        int diffInt, petInt, dayCount;
        if (readValue(in, diffInt, "difficulty") &&
            readValue(in, petInt, "hasPet") &&
            readValue(in, dayCount, "currentDay"))
        {
            info.difficulty = intToDifficulty(diffInt);
            info.hasPet = (petInt != 0);
            info.dayCount = dayCount;
        }

        in.close();
        return info;
    }

} // namespace SaveSystem