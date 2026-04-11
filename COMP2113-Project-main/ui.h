#ifndef UI_H
#define UI_H

#include "game.h"
#include "event.h"
#include <string>

// ============================================================================
// UI NAMESPACE
// ============================================================================

namespace UI {

    // ========================================================================
    // CORE UI FUNCTIONS (User Spec)
    // ========================================================================

    /**
     * Displays title screen with New Game / Load Game / Quit.
     * @return 1=New Game, 2=Load Game, 3=Quit
     */
    int showMainMenu();

    /**
     * Difficulty selection: Story / Easy / Medium / Hard.
     * @return 1=Story, 2=Easy, 3=Medium, 4=Hard
     */
    int showDifficultyMenu();

    /**
     * Asks if player wants a pet companion.
     * @return true if yes
     */
    bool askPetChoice();

    /**
     * Daily action menu: Sail / Explore / Rest (+ Pet Menu if hasPet).
     * @param hasPet true to show pet menu option
     * @return 1=Sail, 2=Explore, 3=Rest, 4=Pet Menu
     */
    int showDailyActionMenu(bool hasPet);

    /**
     * Pet sub-menu: Feed / Play / Status / Back.
     * @return 1=Feed, 2=Play, 3=Status, 4=Back
     */
    int showPetMenu();

    /**
     * Port interaction menu: Buy / Repair / Save / Leave.
     * @return 1=Buy Supplies, 2=Repair Ship, 3=Save Game, 4=Leave Port
     */
    int showPortMenu();

    /**
     * Displays full game status HUD with progress bar.
     * @param game Current game state
     * @param sf Current sanity/fatigue state
     */
    void displayStatus(const GameState& game, const SanityFatigue& sf);

    /**
     * Victory screen.
     * @param daysUsed Total days taken
     * @param bff Whether BFF achievement was unlocked
     */
    void showWinScreen(int daysUsed, bool bff);

    /**
     * Game over screen with defeat reason.
     * @param reason Description of why the player lost
     */
    void showLoseScreen(const std::string& reason);

    /**
     * Validated integer input. Loops until player enters value in [min, max].
     * @return Valid integer in range
     */
    int getInput(int min, int max);

    /**
     * Pauses until player presses Enter.
     */
    void pressEnterToContinue();

    /**
     * Clears terminal using ANSI escape codes.
     */
    void clearScreen();

    // ========================================================================
    // WRAPPER FUNCTIONS (Match main.cpp calling convention)
    // ========================================================================

    /** Displays welcome/title screen. Stores menu choice internally. */
    void showWelcomeScreen();

    /** @return 1 if player chose Load Game, 2 if New Game */
    int askLoadGame();

    /** Wrapper for showDifficultyMenu(). @return difficulty int */
    int chooseDifficulty();

    /** Wrapper for displayStatus(). */
    void showGameStatus(const GameState& game, const SanityFatigue& sf);

    /** Wrapper for showDailyActionMenu() using global pet state. */
    int playerActionMenu();

    /** Prompts player to save. @return 1=Save, 2=No */
    int askSaveGame();

    /** Shows win or lose screen based on game status. */
    void showGameResult(const GameState& game);

} // namespace UI

#endif // UI_H
