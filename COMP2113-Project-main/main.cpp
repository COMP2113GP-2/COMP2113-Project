/*
 * Role 1: Team Leader / Main Program & Integration
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey — The Yokohama Quest
 * Description: Main game loop and program entry point
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "game.h"
#include "ui.h"
#include "event.h"
#include "port.h"
#include "save.h"
#include "pet.h"

using namespace std;

// Global sanity/fatigue state (event system)
SanityFatigue g_sf;

// ============================================================================
// HELPER: Map difficulty menu choice (1-4) to Difficulty enum
// ============================================================================

static Difficulty mapDifficulty(int choice) {
    switch (choice) {
        case 1:  return Difficulty::STORY;
        case 2:  return Difficulty::EASY;
        case 3:  return Difficulty::MEDIUM;
        case 4:  return Difficulty::HARD;
        default: return Difficulty::EASY;
    }
}

// ============================================================================
// HELPER: Print action result summary
// ============================================================================

static void printActionResult(const ActionResult& r) {
    cout << "\n--------------------------------------------" << endl;
    cout << "  " << r.actionName << " Report (" << r.daysSpent << " day"
         << (r.daysSpent != 1 ? "s" : "") << ")" << endl;
    cout << "--------------------------------------------" << endl;

    if (r.distanceGained > 0)
        cout << "  Distance : +" << r.distanceGained << " km" << endl;
    if (r.foodDelta != 0)
        cout << "  Food     : " << r.foodDelta << endl;
    if (r.waterDelta != 0)
        cout << "  Water    : " << r.waterDelta << endl;
    if (r.staminaDelta != 0)
        cout << "  Stamina  : " << (r.staminaDelta > 0 ? "+" : "") << r.staminaDelta << endl;
    if (r.sanityDelta != 0)
        cout << "  Sanity   : " << (r.sanityDelta > 0 ? "+" : "") << r.sanityDelta << endl;
    if (r.durabilityDelta != 0)
        cout << "  Durability: " << r.durabilityDelta << endl;

    cout << "--------------------------------------------\n" << endl;
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    srand(static_cast<unsigned>(time(0)));

    // Step 1: Title screen
    UI::showWelcomeScreen();
    int loadChoice = UI::askLoadGame();

    // Step 2: Determine game parameters
    Difficulty diff = Difficulty::EASY;
    bool hasPet = false;
    bool loaded = false;

    if (loadChoice == 1 && SaveSystem::hasSaveFile()) {
        if (SaveSystem::peekSave(diff, hasPet)) {
            loaded = true;
        }
    }

    if (!loaded) {
        int d = UI::chooseDifficulty();
        diff = mapDifficulty(d);
        PetSystem::initPet();
        hasPet = g_pet.hasPet;
    }

    // Step 3: Create Game object
    Game game(diff, hasPet);
    g_sf = Event::initSanityFatigue(diff);
    Port::resetPortTracker();

    if (loaded) {
        SaveSystem::loadGame(game, g_sf);
    }

    // ====================== MAIN GAME LOOP ======================
    while (game.getStatus() == GameStatus::ONGOING) {
        GameState& state = game.getMutableState();

        // Show status HUD
        UI::showGameStatus(state, g_sf);

        // Player chooses action
        int action = UI::playerActionMenu();

        // Pet menu doesn't consume a turn
        if (action == 4) {
            PetSystem::interactPet(state, g_sf);
            continue;
        }

        // Execute the chosen action
        ActionResult result;
        switch (action) {
            case 1: {
                cout << "  How many days to sail? (2-4): ";
                int days = UI::getInput(2, 4);
                result = game.sail(days);
                break;
            }
            case 2: {
                cout << "  How many days to explore? (1-2): ";
                int days = UI::getInput(1, 2);
                result = game.explore(days);
                break;
            }
            case 3: {
                cout << "  How many days to rest? (1-3): ";
                int days = UI::getInput(1, 3);
                result = game.rest(days);
                break;
            }
        }

        printActionResult(result);

        if (game.getStatus() != GameStatus::ONGOING) break;

        // Update pet daily stats
        PetSystem::updatePet(state, action);

        // Update sanity/fatigue tracking (event system)
        Event::updateFatigue(g_sf, action);
        Event::updateSanity(g_sf, game);

        // Random event check
        if (Event::triggerRandomEvent(game, g_sf)) {
            Event::runRandomEvent(game, g_sf);
        }

        if (game.getStatus() != GameStatus::ONGOING) break;

        // Port encounter
        if (Port::isAtPort(game)) {
            Port::enterPort(game, g_sf);
        }

        // Pet morale bonus (passive sanity lift)
        if (PetSystem::petMoraleBonus(state)) {
            int bonus = 2;
            state.crew.sanity += bonus;
            if (state.crew.sanity > 100) state.crew.sanity = 100;
        }

        // Ask to save
        int saveChoice = UI::askSaveGame();
        if (saveChoice == 1) {
            SaveSystem::saveGame(game, g_sf);
        }
    }

    // Show end screen
    UI::showGameResult(game.getGameState());

    return 0;
}
