/*
 * pet.cpp
 * Role 5: Pet System
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey — The Yokohama Quest
 *
 * Implements the optional pet companion system.
 * The Pet struct (petFood, petMood) lives in game.h.
 * PetState (name, type, alive, BFF, etc.) is defined in pet.h.
 */

#include "pet.h"
#include <iostream>
#include <string>
#include <limits>
#include "ui_display.h"

using namespace std;

// ============================================================================
// GLOBAL PET STATE
// ============================================================================

PetState g_pet;

// ============================================================================
// HELPER: clamp value to [lo, hi]
// ============================================================================

static int clampPet(int value, int lo, int hi) {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

// ============================================================================
// HELPER: safe integer input with validation
// ============================================================================

static int readInt() {
    int value;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// ============================================================================
// CORE PET FUNCTIONS
// ============================================================================

namespace PetSystem {

// ----------------------------------------------------------------------------
// createPet
// ----------------------------------------------------------------------------

PetState createPet() {
    PetState p;
    p.hasPet = true;
    p.alive = true;
    p.daysAsCompanion = 0;
    p.bffUnlocked = false;

    cout << "\n============================================" << endl;
    cout << "       Choose Your Companion!" << endl;
    cout << "============================================" << endl;
    cout << "  [1] Cat    — quiet and mysterious" << endl;
    cout << "  [2] Dog    — loyal and energetic" << endl;
    cout << "  [3] Parrot — chatty and colorful" << endl;
    cout << "============================================" << endl;
    cout << "Pick a pet (1-3): ";

    int choice = readInt();
    while (choice < 1 || choice > 3) {
        cout << "Invalid choice. Pick a pet (1-3): ";
        choice = readInt();
    }

    switch (choice) {
        case 1: p.type = PetType::CAT;    break;
        case 2: p.type = PetType::DOG;    break;
        case 3: p.type = PetType::PARROT; break;
    }

    cout << "Give your companion a name: ";
    getline(cin, p.name);
    while (p.name.empty()) {
        cout << "Name cannot be empty. Try again: ";
        getline(cin, p.name);
    }

    string typeStr;
    switch (p.type) {
        case PetType::CAT:    typeStr = "Cat";    break;
        case PetType::DOG:    typeStr = "Dog";    break;
        case PetType::PARROT: typeStr = "Parrot"; break;
    }

    cout << "\n" << p.name << " the " << typeStr
         << " joins your crew!" << endl;
    cout << "============================================\n" << endl;

    return p;
}

// ----------------------------------------------------------------------------
// updatePetDaily
// ----------------------------------------------------------------------------

void updatePetDaily(GameState& game) {
    if (!g_pet.hasPet || !g_pet.alive || game.pet == nullptr)
        return;

    game.pet->petFood = clampPet(game.pet->petFood - 10, 0, 100);
    game.pet->petMood = clampPet(game.pet->petMood - 5, 0, 100);

    if (game.pet->petFood <= 0 && game.pet->petMood <= 0) {
    g_pet.alive = false;
    Display::showPetDeathNotice(g_pet.name);
    return;
}

    g_pet.daysAsCompanion++;
    checkBFF(game);
}

// ----------------------------------------------------------------------------
// feedPet
// ----------------------------------------------------------------------------

void feedPet(GameState& game) {
    if (!g_pet.hasPet || !g_pet.alive || game.pet == nullptr) {
        cout << "You have no pet to feed." << endl;
        return;
    }

    game.pet->petFood = clampPet(game.pet->petFood + 25, 0, 100);
    game.pet->petMood = clampPet(game.pet->petMood + 5, 0, 100);

    cout << "  You feed " << g_pet.name << " a hearty meal." << endl;
    cout << "  Food +" << 25 << "  Mood +" << 5 << endl;
}

// ----------------------------------------------------------------------------
// interactWithPet
// ----------------------------------------------------------------------------

void interactWithPet(GameState& game) {
    if (!g_pet.hasPet || !g_pet.alive || game.pet == nullptr) {
        cout << "You have no pet to play with." << endl;
        return;
    }

    game.pet->petMood = clampPet(game.pet->petMood + 15, 0, 100);

    switch (g_pet.type) {
        case PetType::CAT:
            cout << "  " << g_pet.name
                 << " purrs softly and curls up in your lap." << endl;
            cout << "  The gentle rumbling eases your worries." << endl;
            break;
        case PetType::DOG:
            cout << "  " << g_pet.name
                 << " fetches a rope and drops it at your feet!" << endl;
            cout << "  A game of tug-of-war lifts everyone's spirits." << endl;
            break;
        case PetType::PARROT:
            cout << "  " << g_pet.name
                 << " squawks: \"Land ho! Just kidding!\"" << endl;
            cout << "  The crew can't help but laugh." << endl;
            break;
    }

    cout << "  Mood +" << 15 << endl;
}

// ----------------------------------------------------------------------------
// checkBFF
// ----------------------------------------------------------------------------

void checkBFF(const GameState& game) {
    if (g_pet.bffUnlocked)
        return;

    if (!g_pet.alive || game.pet == nullptr)
        return;

    if (g_pet.daysAsCompanion >= 10 && game.pet->petMood > 50) {
        g_pet.bffUnlocked = true;
        cout << "\n  *** ACHIEVEMENT UNLOCKED: Best Friend Forever! ***" << endl;
        cout << "  " << g_pet.name
             << " has been your loyal companion for 10 days!" << endl;
    }
}

// ----------------------------------------------------------------------------
// displayPetStatus
// ----------------------------------------------------------------------------

void displayPetStatus(const GameState& game) {
    if (!g_pet.hasPet) {
        cout << "You don't have a pet." << endl;
        return;
    }

    string typeStr;
    switch (g_pet.type) {
        case PetType::CAT:    typeStr = "Cat";    break;
        case PetType::DOG:    typeStr = "Dog";    break;
        case PetType::PARROT: typeStr = "Parrot"; break;
    }

    string statusStr = g_pet.alive ? "Alive" : "Deceased";

    cout << "\n--------------------------------------------" << endl;
    cout << "  Pet Status" << endl;
    cout << "--------------------------------------------" << endl;
    cout << "  Name  : " << g_pet.name << endl;
    cout << "  Type  : " << typeStr << endl;
    cout << "  Status: " << statusStr << endl;

    if (g_pet.alive && game.pet != nullptr) {
        cout << "  Food  : " << game.pet->petFood << " / 100" << endl;
        cout << "  Mood  : " << game.pet->petMood << " / 100" << endl;
    }

    cout << "  Days  : " << g_pet.daysAsCompanion << endl;
    cout << "  BFF   : " << (g_pet.bffUnlocked ? "Unlocked" : "Locked") << endl;
    cout << "--------------------------------------------\n" << endl;
}

// ----------------------------------------------------------------------------
// petMoraleBonus
// ----------------------------------------------------------------------------

bool petMoraleBonus(const GameState& game) {
    if (!g_pet.hasPet || !g_pet.alive || game.pet == nullptr)
        return false;
    return game.pet->petMood > 60;
}

// ============================================================================
// WRAPPER FUNCTIONS (Match main.cpp calling convention)
// ============================================================================

// ----------------------------------------------------------------------------
// initPet — called once at game start
// ----------------------------------------------------------------------------

void initPet() {
    cout << "\n============================================" << endl;
    cout << "  Would you like to bring a pet companion?" << endl;
    cout << "============================================" << endl;
    cout << "  [1] Yes" << endl;
    cout << "  [2] No" << endl;
    cout << "============================================" << endl;
    cout << "Your choice (1-2): ";

    int choice = readInt();
    while (choice < 1 || choice > 2) {
        cout << "Invalid choice. Enter 1 or 2: ";
        choice = readInt();
    }

    if (choice == 1) {
        g_pet = createPet();
    } else {
        g_pet = PetState();
        cout << "\nYou set sail alone. The ocean awaits.\n" << endl;
    }
}

// ----------------------------------------------------------------------------
// updatePet — called each turn from main loop
// ----------------------------------------------------------------------------

void updatePet(GameState& game, int action) {
    (void)action;
    updatePetDaily(game);
}

// ----------------------------------------------------------------------------
// interactPet — called each turn for pet interaction
// ----------------------------------------------------------------------------

void interactPet(GameState& game, SanityFatigue& sf) {
    (void)sf;

    if (!g_pet.hasPet || !g_pet.alive || game.pet == nullptr)
        return;

    cout << "\n============================================" << endl;
    cout << "  " << g_pet.name << " looks at you expectantly..." << endl;
    cout << "============================================" << endl;
    cout << "  [1] Feed " << g_pet.name << endl;
    cout << "  [2] Play with " << g_pet.name << endl;
    cout << "  [3] Check " << g_pet.name << "'s status" << endl;
    cout << "  [4] Maybe later" << endl;
    cout << "============================================" << endl;
    cout << "Your choice (1-4): ";

    int choice = readInt();
    while (choice < 1 || choice > 4) {
        cout << "Invalid choice. Enter 1-4: ";
        choice = readInt();
    }

    switch (choice) {
        case 1:
            feedPet(game);
            break;
        case 2:
            interactWithPet(game);
            break;
        case 3:
            displayPetStatus(game);
            break;
        case 4:
            cout << "  " << g_pet.name << " nuzzles against you briefly." << endl;
            break;
    }

    if (petMoraleBonus(game)) {
        int sanityBoost = 3;
        game.crew.sanity += sanityBoost;
        if (game.crew.sanity > 100) game.crew.sanity = 100;
        cout << "  " << g_pet.name
             << "'s good mood lifts crew spirits! (Sanity +"
             << sanityBoost << ")" << endl;
    }
}

} // namespace PetSystem
