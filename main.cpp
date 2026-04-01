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

int main() {
    // Display welcome screen
    UI::showWelcomeScreen();

    // Initialize game state: load save or new game
    GameState game;
    int loadChoice = UI::askLoadGame();

    if (loadChoice == 1) {
        SaveSystem::loadGame(game);
    } else {
        int difficulty = UI::chooseDifficulty();
        game = Game::initNewGame(difficulty);
    }

    // ====================== MAIN GAME LOOP ======================
    bool gameRunning = true;
    while (gameRunning) {
        // Display current player and journey status
        UI::showGameStatus(game);

        // Let player choose daily action
        int action = UI::playerActionMenu();

        // Execute the chosen action
        Game::executeAction(game, action);

        // Trigger random event if probability conditions are met
        if (Event::triggerRandomEvent()) {
            Event::runRandomEvent(game);
        }

        // Check if reaching a port and handle port interactions
        if (Port::isAtPort(game)) {
            Port::enterPort(game);
        }

        // Pet companion interaction system
        Pet::interactPet(game);

        // Provide save option during the game
        int saveChoice = UI::askSaveGame();
        if (saveChoice == 1) {
            SaveSystem::saveGame(game);
        }

        // Check win/lose conditions and end the game if needed
        if (Game::checkGameOver(game)) {
            UI::showGameResult(game);
            gameRunning = false;
        }
    }

    return 0;
}
