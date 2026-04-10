/*
 * ui.cpp
 * Role 5: UI / Input System
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey — The Yokohama Quest
 *
 * Handles all menus, status displays, input validation,
 * and screen management for the game.
 */

#include "ui.h"
#include "pet.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

// ============================================================================
// MODULE-LOCAL STATE
// ============================================================================

static int s_menuChoice = 0;

// ============================================================================
// CORE UI FUNCTIONS
// ============================================================================

namespace UI {

// ----------------------------------------------------------------------------
// clearScreen
// ----------------------------------------------------------------------------

void clearScreen() {
    cout << "\033[2J\033[1;1H";
}

// ----------------------------------------------------------------------------
// pressEnterToContinue
// ----------------------------------------------------------------------------

void pressEnterToContinue() {
    cout << "  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ----------------------------------------------------------------------------
// getInput
// ----------------------------------------------------------------------------

int getInput(int min, int max) {
    int value;
    while (true) {
        if (cin >> value && value >= min && value <= max) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid input. Enter a number between "
             << min << " and " << max << ": ";
    }
}

// ----------------------------------------------------------------------------
// showMainMenu
// ----------------------------------------------------------------------------

int showMainMenu() {
    clearScreen();
    cout << "============================================" << endl;
    cout << "                                            " << endl;
    cout << "     ABYSSAL ODYSSEY                        " << endl;
    cout << "        The Yokohama Quest                  " << endl;
    cout << "                                            " << endl;
    cout << "     ~  ~  ~  ~  ~  ~  ~  ~  ~             " << endl;
    cout << "       ~~~    ~~~    ~~~    ~~~             " << endl;
    cout << "     ~  ~  ~  ~  ~  ~  ~  ~  ~             " << endl;
    cout << "                                            " << endl;
    cout << "     A Text-Based Ocean Survival Game       " << endl;
    cout << "                                            " << endl;
    cout << "============================================" << endl;
    cout << "  [1] New Game" << endl;
    cout << "  [2] Load Game" << endl;
    cout << "  [3] Quit" << endl;
    cout << "============================================" << endl;
    cout << "  Your choice: ";
    return getInput(1, 3);
}

// ----------------------------------------------------------------------------
// showDifficultyMenu
// ----------------------------------------------------------------------------

int showDifficultyMenu() {
    clearScreen();
    cout << "============================================" << endl;
    cout << "         Select Difficulty" << endl;
    cout << "============================================" << endl;
    cout << "  [1] Story   — No time limit, gentle seas" << endl;
    cout << "  [2] Easy    — 92 day limit, fair winds" << endl;
    cout << "  [3] Medium  — 61 day limit, rough waters" << endl;
    cout << "  [4] Hard    — 41 day limit, brutal ocean" << endl;
    cout << "============================================" << endl;
    cout << "  Your choice: ";
    return getInput(1, 4);
}

// ----------------------------------------------------------------------------
// askPetChoice
// ----------------------------------------------------------------------------

bool askPetChoice() {
    cout << "\n============================================" << endl;
    cout << "  Would you like a pet companion?" << endl;
    cout << "============================================" << endl;
    cout << "  [1] Yes" << endl;
    cout << "  [2] No" << endl;
    cout << "============================================" << endl;
    cout << "  Your choice: ";
    return getInput(1, 2) == 1;
}

// ----------------------------------------------------------------------------
// showDailyActionMenu
// ----------------------------------------------------------------------------

int showDailyActionMenu(bool hasPet) {
    cout << "\n============================================" << endl;
    cout << "         Choose Your Action" << endl;
    cout << "============================================" << endl;
    cout << "  [1] Sail     — Travel toward Yokohama" << endl;
    cout << "  [2] Explore  — Search for resources" << endl;
    cout << "  [3] Rest     — Recover stamina & morale" << endl;
    if (hasPet) {
        cout << "  [4] Pet Menu — Interact with your pet" << endl;
    }
    cout << "============================================" << endl;
    cout << "  Your choice: ";

    int maxChoice = hasPet ? 4 : 3;
    return getInput(1, maxChoice);
}

// ----------------------------------------------------------------------------
// showPetMenu
// ----------------------------------------------------------------------------

int showPetMenu() {
    cout << "\n--------------------------------------------" << endl;
    cout << "         Pet Menu" << endl;
    cout << "--------------------------------------------" << endl;
    cout << "  [1] Feed" << endl;
    cout << "  [2] Play" << endl;
    cout << "  [3] Status" << endl;
    cout << "  [4] Back" << endl;
    cout << "--------------------------------------------" << endl;
    cout << "  Your choice: ";
    return getInput(1, 4);
}

// ----------------------------------------------------------------------------
// showPortMenu
// ----------------------------------------------------------------------------

int showPortMenu() {
    cout << "\n============================================" << endl;
    cout << "         Port Services" << endl;
    cout << "============================================" << endl;
    cout << "  [1] Buy Supplies" << endl;
    cout << "  [2] Repair Ship" << endl;
    cout << "  [3] Save Game" << endl;
    cout << "  [4] Leave Port" << endl;
    cout << "============================================" << endl;
    cout << "  Your choice: ";
    return getInput(1, 4);
}

// ----------------------------------------------------------------------------
// displayStatus
// ----------------------------------------------------------------------------

void displayStatus(const GameState& game, const SanityFatigue& sf) {
    clearScreen();

    // Day and distance header
    int daysLimit = -1;
    switch (game.difficulty) {
        case Difficulty::STORY:  daysLimit = -1; break;
        case Difficulty::EASY:   daysLimit = 92; break;
        case Difficulty::MEDIUM: daysLimit = 61; break;
        case Difficulty::HARD:   daysLimit = 41; break;
    }

    cout << "============================================" << endl;
    cout << "  VOYAGE STATUS" << endl;
    cout << "============================================" << endl;

    // Day counter
    cout << "  Day: " << game.currentDay;
    if (daysLimit > 0) {
        cout << " / " << daysLimit;
    }
    cout << endl;

    // Distance progress bar (30 chars wide)
    int dist = game.ship.distance;
    if (dist > 3000) dist = 3000;
    int filled = (dist * 30) / 3000;
    int empty  = 30 - filled;

    cout << "  Distance: " << game.ship.distance << " / 3000 km" << endl;
    cout << "  [";
    for (int i = 0; i < filled; i++) cout << "=";
    if (filled < 30) {
        cout << ">";
        for (int i = 1; i < empty; i++) cout << "-";
    }
    cout << "] " << (dist * 100) / 3000 << "%" << endl;

    cout << "--------------------------------------------" << endl;

    // Resources
    cout << "  Food       : " << game.resources.food << endl;
    cout << "  Water      : " << game.resources.freshWater << endl;
    cout << "  Gold       : " << game.resources.gold << endl;

    cout << "--------------------------------------------" << endl;

    // Crew stats
    cout << "  Stamina    : " << game.crew.stamina << " / 10" << endl;
    cout << "  Sanity     : " << game.crew.sanity  << " / 100";
    if (game.crew.sanity < 40) {
        cout << "  [CRITICAL]";
    } else if (game.crew.sanity >= 80) {
        cout << "  [GOOD]";
    }
    cout << endl;

    // Ship stats
    cout << "  Durability : " << game.ship.durability << " / 10" << endl;

    // Fatigue indicator
    if (sf.consecutiveSailingDays >= 5) {
        cout << "  Fatigue    : FATIGUED ("
             << sf.consecutiveSailingDays << " consecutive sail days)" << endl;
    } else if (sf.consecutiveSailingDays >= 3) {
        cout << "  Fatigue    : Warning ("
             << sf.consecutiveSailingDays << " consecutive sail days)" << endl;
    }

    // Pet summary (brief, if alive)
    if (g_pet.hasPet && g_pet.alive && game.pet != nullptr) {
        cout << "--------------------------------------------" << endl;
        cout << "  Pet (" << g_pet.name << "): "
             << "Food " << game.pet->petFood
             << " | Mood " << game.pet->petMood << endl;
    }

    cout << "============================================\n" << endl;
}

// ----------------------------------------------------------------------------
// showWinScreen
// ----------------------------------------------------------------------------

void showWinScreen(int daysUsed, bool bff) {
    clearScreen();
    cout << "============================================" << endl;
    cout << "                                            " << endl;
    cout << "         *** VICTORY! ***                   " << endl;
    cout << "                                            " << endl;
    cout << "   You have reached Yokohama!               " << endl;
    cout << "   The Abyssal Odyssey is complete.         " << endl;
    cout << "                                            " << endl;
    cout << "   Days at sea: " << daysUsed << endl;
    cout << "                                            " << endl;
    if (bff) {
        cout << "   *** ACHIEVEMENT: Best Friend Forever! ***" << endl;
        cout << "   Your pet stayed by your side through     " << endl;
        cout << "   every storm and starlit night.           " << endl;
        cout << "                                            " << endl;
    }
    cout << "   Thank you for playing!                   " << endl;
    cout << "                                            " << endl;
    cout << "============================================" << endl;
}

// ----------------------------------------------------------------------------
// showLoseScreen
// ----------------------------------------------------------------------------

void showLoseScreen(const string& reason) {
    clearScreen();
    cout << "============================================" << endl;
    cout << "                                            " << endl;
    cout << "         *** GAME OVER ***                  " << endl;
    cout << "                                            " << endl;
    cout << "   The ocean claims another voyage...       " << endl;
    cout << "                                            " << endl;
    cout << "   Reason: " << reason << endl;
    cout << "                                            " << endl;
    cout << "   Better luck next time, Captain.          " << endl;
    cout << "                                            " << endl;
    cout << "============================================" << endl;
}

// ============================================================================
// WRAPPER FUNCTIONS (Match main.cpp calling convention)
// ============================================================================

// ----------------------------------------------------------------------------
// showWelcomeScreen
// ----------------------------------------------------------------------------

void showWelcomeScreen() {
    s_menuChoice = showMainMenu();
    if (s_menuChoice == 3) {
        cout << "\n  Fair winds, Captain. Until next time.\n" << endl;
        exit(0);
    }
}

// ----------------------------------------------------------------------------
// askLoadGame — returns 1 for load, 2 for new game
// main.cpp checks: if (loadChoice == 1) => load
// ----------------------------------------------------------------------------

int askLoadGame() {
    // showMainMenu returns 1=New, 2=Load, 3=Quit
    // main.cpp expects 1=Load, else=New
    if (s_menuChoice == 2) return 1;  // Load Game
    return 2;                          // New Game
}

// ----------------------------------------------------------------------------
// chooseDifficulty
// ----------------------------------------------------------------------------

int chooseDifficulty() {
    return showDifficultyMenu();
}

// ----------------------------------------------------------------------------
// showGameStatus
// ----------------------------------------------------------------------------

void showGameStatus(const GameState& game, const SanityFatigue& sf) {
    displayStatus(game, sf);
}

// ----------------------------------------------------------------------------
// playerActionMenu
// ----------------------------------------------------------------------------

int playerActionMenu() {
    return showDailyActionMenu(g_pet.hasPet);
}

// ----------------------------------------------------------------------------
// askSaveGame
// ----------------------------------------------------------------------------

int askSaveGame() {
    cout << "\n--------------------------------------------" << endl;
    cout << "  Save your progress?" << endl;
    cout << "--------------------------------------------" << endl;
    cout << "  [1] Yes" << endl;
    cout << "  [2] No" << endl;
    cout << "--------------------------------------------" << endl;
    cout << "  Your choice: ";
    return getInput(1, 2);
}

// ----------------------------------------------------------------------------
// showGameResult
// ----------------------------------------------------------------------------

void showGameResult(const GameState& game) {
    if (game.status == GameStatus::VICTORY) {
        showWinScreen(game.currentDay, g_pet.bffUnlocked);
    } else {
        string reason;
        if (game.crew.stamina <= 0) {
            reason = "Your crew collapsed from exhaustion.";
        } else if (game.ship.durability <= 0) {
            reason = "Your ship broke apart beneath the waves.";
        } else if (game.daysWithoutWaterResupply >= 2) {
            reason = "The crew perished from dehydration.";
        } else {
            reason = "You ran out of time to reach Yokohama.";
        }
        showLoseScreen(reason);
    }
}

} // namespace UI
