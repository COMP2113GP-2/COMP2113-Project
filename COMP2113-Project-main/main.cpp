/*
 * main.cpp
 * Role 1: Team Leader / Main Program & Integration
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstdio>
#include "ui_display.h"
#include "game.h"
#include "ui.h"
#include "event.h"
#include "port.h"
#include "save.h"
#include "pet.h"

using namespace std;

SanityFatigue g_sf;

// ============================================================================
// HELPER: Map difficulty menu choice (1-4) to Difficulty enum
// ============================================================================

static Difficulty mapDifficulty(int choice)
{
    switch (choice)
    {
    case 1:
        return Difficulty::STORY;
    case 2:
        return Difficulty::EASY;
    case 3:
        return Difficulty::MEDIUM;
    case 4:
        return Difficulty::HARD;
    default:
        return Difficulty::EASY;
    }
}

// ============================================================================
// HELPER: Print action result summary to console
// Input:  r — ActionResult from sail/rest/explore
// Output: formatted summary printed to stdout
// ============================================================================

static void printActionResult(const ActionResult &r)
{
    cout << "\n--------------------------------------------\n";
    cout << "  " << r.actionName << " Report ("
         << r.daysSpent << " day"
         << (r.daysSpent != 1 ? "s" : "") << ")\n";
    cout << "--------------------------------------------\n";
    if (r.distanceGained > 0)
        cout << "  Distance  : +" << r.distanceGained << " km\n";
    if (r.foodDelta != 0)
        cout << "  Food      : " << (r.foodDelta > 0 ? "+" : "") << r.foodDelta << "\n";
    if (r.waterDelta != 0)
        cout << "  Water     : " << (r.waterDelta > 0 ? "+" : "") << r.waterDelta << "\n";
    if (r.staminaDelta != 0)
        cout << "  Stamina   : " << (r.staminaDelta > 0 ? "+" : "") << r.staminaDelta << "\n";
    if (r.sanityDelta != 0)
        cout << "  Sanity    : " << (r.sanityDelta > 0 ? "+" : "") << r.sanityDelta << "\n";
    if (r.durabilityDelta != 0)
        cout << "  Durability: " << (r.durabilityDelta > 0 ? "+" : "") << r.durabilityDelta << "\n";
    cout << "--------------------------------------------\n\n";
}

// ============================================================================
// HELPER: Build compact one-line outcome summary for HUD top bar
// Input:  r — ActionResult from sail/rest/explore
// Output: string like "Sail (3d) | +250km | Food -6 | Water -6"
// ============================================================================

static string buildOutcomeLine(const ActionResult &r)
{
    string s = r.actionName + " (" + to_string(r.daysSpent) + "d)";
    if (r.distanceGained > 0)
        s += " | +" + to_string(r.distanceGained) + "km";
    if (r.foodDelta != 0)
        s += string(" | Food ") + (r.foodDelta > 0 ? "+" : "") + to_string(r.foodDelta);
    if (r.waterDelta != 0)
        s += string(" | Water ") + (r.waterDelta > 0 ? "+" : "") + to_string(r.waterDelta);
    if (r.staminaDelta != 0)
        s += string(" | Stamina ") + (r.staminaDelta > 0 ? "+" : "") + to_string(r.staminaDelta);
    if (r.sanityDelta != 0)
        s += string(" | Sanity ") + (r.sanityDelta > 0 ? "+" : "") + to_string(r.sanityDelta);
    if (r.durabilityDelta != 0)
        s += string(" | Dur ") + (r.durabilityDelta > 0 ? "+" : "") + to_string(r.durabilityDelta);
    return s;
}

// ============================================================================
// MAIN
// ============================================================================

int main()
{
    srand(static_cast<unsigned>(time(0)));

    Display::showWidthReminder(); 

    while (true) {

        // Step 1: Title + main menu
        UI::showWelcomeScreen();
        int loadChoice = UI::askLoadGame();

        // Handle Delete Save option from main menu
        if (loadChoice == 3)
        {
            UI::askDeleteSave();
            continue; // Loop back to main menu
        }

        // Offer to delete save file if starting new game and save exists
        if (loadChoice == 2 && SaveSystem::hasSaveFile())
        {
            int deleteChoice = UI::askDeleteSave();
            // If player chose to delete, the function already called deleteSaveFile()
        }

        // Step 2: Determine difficulty and pet
        Difficulty diff = Difficulty::EASY;
        bool hasPet = false;
        bool loaded = false;

        if (loadChoice == 1 && SaveSystem::hasSaveFile())
        {
            if (SaveSystem::peekSave(diff, hasPet))
                loaded = true;
        }

        if (!loaded)
        {
            int d = UI::chooseDifficulty();
            diff = mapDifficulty(d);
            PetSystem::initPet();
            hasPet = g_pet.hasPet;
        }

        // Step 3: Construct game objects
        Game game(diff, hasPet);
        g_sf = Event::initSanityFatigue(diff);
        Port::resetPortTracker();

        if (loaded)
            SaveSystem::loadGame(game, g_sf);

        // ====================== MAIN GAME LOOP ======================
        while (game.getStatus() == GameStatus::ONGOING)
        {
            GameState &state = game.getMutableState();

            // Show HUD (last outcome shown at top if set)
            UI::showGameStatus(state, g_sf);

            // Player chooses action
            int action = UI::playerActionMenu();

            // Pet menu — does not consume a turn
            if (action == 4)
            {
                PetSystem::interactPet(state, g_sf);
                continue;
            }

            // Execute chosen action, play animation, build outcome string
            ActionResult result;
            string outcomeStr;

            switch (action)
            {
            case 1:
            {
                cout << "  How many days to sail? (2-4): ";
                int days = UI::getInput(2, 4);
                result = game.sail(days);
                printActionResult(result);
                Display::sailAnimation(result.distanceGained, result.daysSpent);
                outcomeStr = buildOutcomeLine(result);
                break;
            }
            case 2:
            {
                cout << "  How many days to explore? (1-2): ";
                int days = UI::getInput(1, 2);
                result = game.explore(days);
                Display::exploreAnimation();
                printActionResult(result);
                outcomeStr = buildOutcomeLine(result);
                break;
            }
            case 3:
            {
                cout << "  How many days to rest? (1-3): ";
                int days = UI::getInput(1, 3);
                result = game.rest(days);
                Display::restAnimation();
                printActionResult(result);
                outcomeStr = buildOutcomeLine(result);
                break;
            }
            default:
                break;
            }

            if (game.getStatus() != GameStatus::ONGOING)
                break;

            // Update pet, fatigue, sanity tracking
            PetSystem::updatePet(state, action);
            Event::updateFatigue(g_sf, action);
            Event::updateSanity(g_sf, game);

            // Random event check — fires after Sail or Explore
            if (action == 1 || action == 2) {
                if (Event::triggerRandomEvent(game, g_sf)) {
                    EventResult evResult = Event::runRandomEventWithResult(game, g_sf);

                    // Apply extra days lost
                    if (evResult.extraDaysLost > 0) {
                        game.getMutableState().currentDay += evResult.extraDaysLost;
                        game.getMutableState().daysWithoutWaterResupply +=
                            evResult.extraDaysLost;
                    }

                    // Apply distance bonus
                    if (evResult.extraDaysLost < 0) {
                        game.getMutableState().ship.distance +=
                            (-evResult.extraDaysLost);
                        if (game.getMutableState().ship.distance > 3000)
                            game.getMutableState().ship.distance = 3000;
                    }

                    // Half distance for Reef choice B
                    if (evResult.halfDistanceThisTurn && result.distanceGained > 0) {
                        game.getMutableState().ship.distance -=
                            (result.distanceGained / 2);
                        if (game.getMutableState().ship.distance < 0)
                            game.getMutableState().ship.distance = 0;
                    }
                    
                    // Wait for player to read event before HUD clears
                    cout << "\n\033[33m\033[1m  Press Enter to continue...\033[0m" << flush;
                    {
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF)
                        {
                        }
                    }
                    outcomeStr += " | Event fired!";
                }
            }

            if (game.getStatus() != GameStatus::ONGOING)
                break;

            // Port encounter check
            if (Port::isAtPort(game))
            {
                Port::enterPort(game, g_sf);
            }

            if (game.getStatus() != GameStatus::ONGOING)
                break;

            // Passive pet morale sanity bonus
            if (PetSystem::petMoraleBonus(state))
            {
                state.crew.sanity += 2;
                if (state.crew.sanity > 100)
                    state.crew.sanity = 100;
                outcomeStr += " | Pet +2 Sanity";
            }

            // Store outcome for display at top of next HUD
            UI::setLastOutcome(outcomeStr);

            // Save prompt
            int saveChoice = UI::askSaveGame();
            if (saveChoice == 1)
            {
                SaveSystem::saveGame(game, g_sf);
            }
        }

        // Show end screen
        UI::showGameResult(game.getGameState());
    }

    return 0;
}