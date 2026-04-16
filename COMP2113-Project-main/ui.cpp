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
#include "ui_display.h"
#include "pet.h"
#include "save.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

// ============================================================================
// COLOR DEFINITIONS
// ============================================================================

// ANSI color
#define COLOR_RESET "\033[0m"
#define COLOR_BOLD "\033[1m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"

#define COLOR_TITLE "\033[1;36m"
#define COLOR_HEADER "\033[1;35m"
#define COLOR_OPTION "\033[1;33m"
#define COLOR_LABEL "\033[1;36m"
#define COLOR_VALUE "\033[1;37m"
#define COLOR_GOOD "\033[1;32m"
#define COLOR_WARNING "\033[1;33m"
#define COLOR_DANGER "\033[1;31m"
#define COLOR_PROMPT "\033[1;33m"

// ============================================================================
// MODULE-LOCAL STATE
// ============================================================================

static int s_menuChoice = 0;

// ============================================================================
// CORE UI FUNCTIONS
// ============================================================================
static string g_lastOutcome = "";

namespace UI
{

    void setLastOutcome(const string &s) { g_lastOutcome = s; }

    // ----------------------------------------------------------------------------
    // clearScreen
    // ----------------------------------------------------------------------------

    void clearScreen()
    {
        cout << "\033[2J\033[1;1H";
    }

    // ----------------------------------------------------------------------------
    // pressEnterToContinue
    // ----------------------------------------------------------------------------

    void pressEnterToContinue()
    {
        cout << COLOR_PROMPT << "  Press Enter to continue..." << COLOR_RESET;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // ----------------------------------------------------------------------------
    // getInput
    // ----------------------------------------------------------------------------

    int getInput(int min, int max)
    {
        int value;
        while (true)
        {
            cout << COLOR_PROMPT;
            if (cin >> value && value >= min && value <= max)
            {
                cout << COLOR_RESET;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << COLOR_DANGER << "  Invalid input. Enter a number between "
                 << min << " and " << max << ": " << COLOR_RESET;
        }
    }

    // ----------------------------------------------------------------------------
    // showMainMenu
    // ----------------------------------------------------------------------------

    int showMainMenu()
    {
        clearScreen();
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << "                                            " << endl;
        cout << COLOR_TITLE << "     ABYSSAL ODYSSEY" << COLOR_RESET << endl;
        cout << COLOR_HEADER << "        The Yokohama Quest" << COLOR_RESET << endl;
        cout << "                                            " << endl;
        cout << COLOR_CYAN << "     ~  ~  ~  ~  ~  ~  ~  ~  ~             " << COLOR_RESET << endl;
        cout << COLOR_CYAN << "       ~~~    ~~~    ~~~    ~~~             " << COLOR_RESET << endl;
        cout << COLOR_CYAN << "     ~  ~  ~  ~  ~  ~  ~  ~  ~             " << COLOR_RESET << endl;
        cout << "                                            " << endl;
        cout << "     A Text-Based Ocean Survival Game       " << endl;
        cout << "                                            " << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [1] New Game" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [2] Load Game" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [3] Delete Save" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [4] Quit" << COLOR_RESET << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_PROMPT << "  Your choice: " << COLOR_RESET;
        return getInput(1, 4);
    }

    // ----------------------------------------------------------------------------
    // showDifficultyMenu
    // ----------------------------------------------------------------------------

    int showDifficultyMenu()
    {
        clearScreen();
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_HEADER << "         Select Difficulty" << COLOR_RESET << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [1] Story   " << COLOR_RESET << "- No time limit, gentle seas" << endl;
        cout << COLOR_OPTION << "  [2] Easy    " << COLOR_RESET << "- 92 day limit, fair winds" << endl;
        cout << COLOR_OPTION << "  [3] Medium  " << COLOR_RESET << "- 61 day limit, rough waters" << endl;
        cout << COLOR_OPTION << "  [4] Hard    " << COLOR_RESET << "- 41 day limit, brutal ocean" << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_PROMPT << "  Your choice: " << COLOR_RESET;
        return getInput(1, 4);
    }

    // ----------------------------------------------------------------------------
    // askPetChoice
    // ----------------------------------------------------------------------------

    bool askPetChoice()
    {
        cout << endl
             << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_HEADER << "  Would you like a pet companion?" << COLOR_RESET << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [1] Yes" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [2] No" << COLOR_RESET << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_PROMPT << "  Your choice: " << COLOR_RESET;
        return getInput(1, 2) == 1;
    }

    // ----------------------------------------------------------------------------
    // showDailyActionMenu
    // ----------------------------------------------------------------------------

    int showDailyActionMenu(bool hasPet)
    {
        cout << endl
             << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_HEADER << "         Choose Your Action" << COLOR_RESET << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [1] Sail     " << COLOR_RESET << "- Travel toward Yokohama" << endl;
        cout << COLOR_OPTION << "  [2] Explore  " << COLOR_RESET << "- Search for resources" << endl;
        cout << COLOR_OPTION << "  [3] Rest     " << COLOR_RESET << "- Recover stamina & morale" << endl;
        if (hasPet)
        {
            cout << COLOR_OPTION << "  [4] Pet Menu " << COLOR_RESET << "- Interact with your pet" << endl;
        }
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_PROMPT << "  Your choice: " << COLOR_RESET;

        int maxChoice = hasPet ? 4 : 3;
        return getInput(1, maxChoice);
    }

    // ----------------------------------------------------------------------------
    // showPetMenu
    // ----------------------------------------------------------------------------

    int showPetMenu()
    {
        cout << endl
             << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
        cout << COLOR_HEADER << "         Pet Menu" << COLOR_RESET << endl;
        cout << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [1] Feed" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [2] Play" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [3] Status" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [4] Back" << COLOR_RESET << endl;
        cout << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
        cout << COLOR_PROMPT << "  Your choice: " << COLOR_RESET;
        return getInput(1, 4);
    }

    // ----------------------------------------------------------------------------
    // showPortMenu
    // ----------------------------------------------------------------------------

    int showPortMenu()
    {
        cout << endl
             << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_HEADER << "         Port Services" << COLOR_RESET << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [1] Buy Supplies" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [2] Repair Ship" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [3] Save Game" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [4] Leave Port" << COLOR_RESET << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_PROMPT << "  Your choice: " << COLOR_RESET;
        return getInput(1, 4);
    }

    // ----------------------------------------------------------------------------
    // displayStatus
    // ----------------------------------------------------------------------------

    void displayStatus(const GameState &game, const SanityFatigue &sf)
    {
        clearScreen();

        // Day and distance header
        int daysLimit = -1;
        switch (game.difficulty)
        {
        case Difficulty::STORY:
            daysLimit = -1;
            break;
        case Difficulty::EASY:
            daysLimit = 92;
            break;
        case Difficulty::MEDIUM:
            daysLimit = 61;
            break;
        case Difficulty::HARD:
            daysLimit = 41;
            break;
        }

        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << COLOR_HEADER << "  VOYAGE STATUS" << COLOR_RESET << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;

        // Day counter
        cout << COLOR_LABEL << "  Day: " << COLOR_VALUE << game.currentDay;
        if (daysLimit > 0)
        {
            cout << COLOR_LABEL << " / " << COLOR_VALUE << daysLimit;
        }
        cout << COLOR_RESET << endl;

        // Distance progress bar (30 chars wide)
        int dist = game.ship.distance;
        if (dist > 3000)
            dist = 3000;
        int filled = (dist * 30) / 3000;
        int empty = 30 - filled;

        cout << COLOR_LABEL << "  Distance: " << COLOR_VALUE << game.ship.distance
             << COLOR_LABEL << " / 3000 km" << COLOR_RESET << endl;
        cout << "  [";
        cout << COLOR_GREEN;
        for (int i = 0; i < filled; i++)
            cout << "=";
        if (filled < 30)
        {
            cout << ">";
            for (int i = 1; i < empty; i++)
                cout << "-";
        }
        cout << COLOR_RESET << "] " << COLOR_VALUE << (dist * 100) / 3000 << "%" << COLOR_RESET << endl;

        cout << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;

        // Resources
        cout << COLOR_LABEL << "  Food       : " << COLOR_VALUE << game.resources.food << COLOR_RESET << endl;
        cout << COLOR_LABEL << "  Water      : " << COLOR_VALUE << game.resources.freshWater << COLOR_RESET << endl;
        cout << COLOR_LABEL << "  Gold       : " << COLOR_YELLOW << game.resources.gold << COLOR_RESET << endl;

        cout << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;

        // Crew stats
        cout << COLOR_LABEL << "  Stamina    : " << COLOR_VALUE << game.crew.stamina
             << COLOR_LABEL << " / 10" << COLOR_RESET << endl;

        cout << COLOR_LABEL << "  Sanity     : ";
        if (game.crew.sanity < 40)
        {
            cout << COLOR_DANGER << game.crew.sanity << " / 100  [CRITICAL]" << COLOR_RESET;
        }
        else if (game.crew.sanity >= 80)
        {
            cout << COLOR_GOOD << game.crew.sanity << " / 100  [GOOD]" << COLOR_RESET;
        }
        else
        {
            cout << COLOR_WARNING << game.crew.sanity << " / 100" << COLOR_RESET;
        }
        cout << endl;

        // Ship stats
        cout << COLOR_LABEL << "  Durability : " << COLOR_VALUE << game.ship.durability
             << COLOR_LABEL << " / 10" << COLOR_RESET << endl;

        // Fatigue indicator
        if (sf.consecutiveSailingDays >= 5)
        {
            cout << COLOR_DANGER << "  Fatigue    : FATIGUED ("
                 << sf.consecutiveSailingDays << " consecutive sail days)" << COLOR_RESET << endl;
        }
        else if (sf.consecutiveSailingDays >= 3)
        {
            cout << COLOR_WARNING << "  Fatigue    : Warning ("
                 << sf.consecutiveSailingDays << " consecutive sail days)" << COLOR_RESET << endl;
        }

        // Pet summary (brief, if alive)
        if (g_pet.hasPet && g_pet.alive && game.pet != nullptr)
        {
            cout << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
            cout << COLOR_LABEL << "  Pet (" << COLOR_MAGENTA << g_pet.name << COLOR_LABEL << "): "
                 << COLOR_VALUE << "Food " << game.pet->petFood
                 << COLOR_LABEL << " | Mood " << COLOR_VALUE << game.pet->petMood << COLOR_RESET << endl;
        }

        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl
             << endl;
    }

    // ----------------------------------------------------------------------------
    // showWinScreen
    // ----------------------------------------------------------------------------

    void showWinScreen(int daysUsed, bool bff)
    {
        clearScreen();
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << "                                            " << endl;
        cout << COLOR_GOOD << "         *** VICTORY! ***                   " << COLOR_RESET << endl;
        cout << "                                            " << endl;
        cout << COLOR_GREEN << "   You have reached Yokohama!               " << COLOR_RESET << endl;
        cout << "   The Abyssal Odyssey is complete.         " << endl;
        cout << "                                            " << endl;
        cout << "   Days at sea: " << COLOR_VALUE << daysUsed << COLOR_RESET << endl;
        cout << "                                            " << endl;
        if (bff)
        {
            cout << COLOR_MAGENTA << "   *** ACHIEVEMENT: Best Friend Forever! ***" << COLOR_RESET << endl;
            cout << "   Your pet stayed by your side through     " << endl;
            cout << "   every storm and starlit night.           " << endl;
            cout << "                                            " << endl;
        }
        cout << "   Thank you for playing!                   " << endl;
        cout << "                                            " << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
    }

    // ----------------------------------------------------------------------------
    // showLoseScreen
    // ----------------------------------------------------------------------------

    void showLoseScreen(const string &reason)
    {
        clearScreen();
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
        cout << "                                            " << endl;
        cout << COLOR_DANGER << "         *** GAME OVER ***                  " << COLOR_RESET << endl;
        cout << "                                            " << endl;
        cout << "   The ocean claims another voyage...       " << endl;
        cout << "                                            " << endl;
        cout << COLOR_WARNING << "   Reason: " << reason << COLOR_RESET << endl;
        cout << "                                            " << endl;
        cout << "   Better luck next time, Captain.          " << endl;
        cout << "                                            " << endl;
        cout << COLOR_TITLE << "============================================" << COLOR_RESET << endl;
    }

    // ============================================================================
    // WRAPPER FUNCTIONS (Match main.cpp calling convention)
    // ============================================================================

    // ----------------------------------------------------------------------------
    // showWelcomeScreen
    // ----------------------------------------------------------------------------

    void showWelcomeScreen()
    {
        Display::showTitleBanner();
        s_menuChoice = showMainMenu();
        if (s_menuChoice == 4)
        {
            cout << endl
                 << COLOR_CYAN << "  Fair winds, Captain. Until next time." << COLOR_RESET << endl
                 << endl;
            exit(0);
        }
    }

    // ----------------------------------------------------------------------------
    // askLoadGame — returns 1 for load, 2 for new game
    // main.cpp checks: if (loadChoice == 1) => load
    // ----------------------------------------------------------------------------

    int askLoadGame()
    {
        // showMainMenu returns 1=New, 2=Load, 3=Delete, 4=Quit
        // main.cpp expects: 1=Load, 2=New, 3=Delete
        if (s_menuChoice == 2)
            return 1; // Load Game
        if (s_menuChoice == 3)
            return 3; // Delete Save
        return 2;     // New Game
    }

    // ----------------------------------------------------------------------------
    // chooseDifficulty
    // ----------------------------------------------------------------------------

    int chooseDifficulty()
    {
        int choice = showDifficultyMenu();
        // Default to Easy (2) if invalid choice
        if (choice < 1 || choice > 4)
        {
            cout << COLOR_WARNING << "  Invalid choice. Defaulting to Easy difficulty." << COLOR_RESET << endl;
            return 2;
        }
        return choice;
    }

    // ----------------------------------------------------------------------------
    // showGameStatus
    // ----------------------------------------------------------------------------

    void showGameStatus(const GameState &game, const SanityFatigue &sf)
    {
        Display::showGameHUD(game, sf, g_lastOutcome);
        g_lastOutcome = "";
    }

    // ----------------------------------------------------------------------------
    // playerActionMenu
    // ----------------------------------------------------------------------------

    int playerActionMenu()
    {
        return showDailyActionMenu(g_pet.hasPet);
    }

    // ----------------------------------------------------------------------------
    // askSaveGame
    // ----------------------------------------------------------------------------

    int askSaveGame()
    {
        cout << endl
             << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
        cout << COLOR_HEADER << "  Save your progress?" << COLOR_RESET << endl;
        cout << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [1] Yes" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [2] No" << COLOR_RESET << endl;
        cout << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
        cout << COLOR_PROMPT << "  Your choice: " << COLOR_RESET;
        return getInput(1, 2);
    }

    // ----------------------------------------------------------------------------
    // askDeleteSave
    // ----------------------------------------------------------------------------

    int askDeleteSave()
    {
        cout << endl
             << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
        cout << COLOR_HEADER << "  Delete save file?" << COLOR_RESET << endl;
        cout << COLOR_WARNING << "  This cannot be undone!" << COLOR_RESET << endl;
        cout << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [1] Delete" << COLOR_RESET << endl;
        cout << COLOR_OPTION << "  [2] Cancel" << COLOR_RESET << endl;
        cout << COLOR_CYAN << "--------------------------------------------" << COLOR_RESET << endl;
        cout << COLOR_PROMPT << "  Your choice: " << COLOR_RESET;
        int choice = getInput(1, 2);

        if (choice == 1)
        {
            SaveSystem::deleteSaveFile();
        }

        return choice;
    }

    // ----------------------------------------------------------------------------
    // showGameResult
    // ----------------------------------------------------------------------------

    void showGameResult(const GameState &game)
    {
        if (game.status == GameStatus::VICTORY)
        {
            Display::victoryScreen(game.currentDay, g_pet.bffUnlocked);
        }
        else
        {
            string reason;
            if (game.crew.stamina <= 0)
                reason = "Your crew collapsed from exhaustion.";
            else if (game.ship.durability <= 0)
                reason = "Your ship broke apart beneath the waves.";
            else if (game.daysWithoutWaterResupply >= 2)
                reason = "The crew perished from dehydration.";
            else
                reason = "You ran out of time to reach Yokohama.";
            Display::defeatScreen(reason);
        }
    }

} // namespace UI
