/*
 * Role 1: Team Leader / Main Program & Integration
 * Course: COMP2113 / ENGG1340
 * Project: Ocean Voyage
 * Description: Main game loop and program entry point
 */

#include <iostream>
#include "game.h"
#include "ui.h"
#include "event.h"
#include "port.h"
#include "save.h"
#include "pet.h"

using namespace std;

// Global objects (link Role2-Role5)
extern SanityFatigue g_sf;
extern PetState g_pet;

int main() {
    // Step 1: Show welcome screen (Role5)
    UI::showWelcomeScreen();

    // Step 2: Ask load game or new game (Role5)
    GameState game;
    int loadChoice = UI::askLoadGame();

    if (loadChoice == 1) {
        // Load game (Role4: save/load)
        if (!SaveSystem::loadGame(game, g_sf)) {
            // Load failed → init new game
            int difficulty = UI::chooseDifficulty();
            game = Game::initNewGame(difficulty);
            g_sf = Event::initSanityFatigue(difficulty);
            Pet::initPet(); // Role5: pet init
        }
    } else {
        // New game: init all modules
        int difficulty = UI::chooseDifficulty();
        game = Game::initNewGame(difficulty); // Role2: core state
        g_sf = Event::initSanityFatigue(difficulty); // Role3: sanity/fatigue
        Pet::initPet(); // Role5: pet system (player choose carry or not)
    }

    // ====================== MAIN GAME LOOP ======================
    bool gameRunning = true;
    while (gameRunning) {
        // Step 1: Show real-time game status (Role5)
        UI::showGameStatus(game, g_sf);

        // Step 2: Player choose daily action (Role5)
        int action = UI::playerActionMenu();

        // Step 3: Execute core game action (Role2)
        Game::executeAction(game, action);

        // Step 4: Update pet state (Role5: sail food consume/mood drain)
        Pet::updatePet(game, action);

        // Step 5: Update sanity/fatigue (Role3: core event rule)
        Event::updateFatigue(g_sf, action);
        Event::updateSanity(g_sf, game);

        // Step 6: Trigger random event (Role3)
        if (Event::triggerRandomEvent(game, g_sf)) {
            Event::runRandomEvent(game, g_sf);
        }

        // Step 7: Check port encounter and interact (Role4)
        if (Port::isAtPort(game)) {
            Port::enterPort(game, g_sf);
        }

        // Step 8: Pet interaction (Role5: play with pet, rest bonus)
        Pet::interactPet(game, g_sf);

        // Step 9: Ask save game (Role5 + Role4)
        int saveChoice = UI::askSaveGame();
        if (saveChoice == 1) {
            SaveSystem::saveGame(game, g_sf);
        }

        // Step 10: Check game over (Role2) and show result (Role5)
        if (Game::checkGameOver(game)) {
            UI::showGameResult(game);
            gameRunning = false;
        }
    }

    return 0;
}
