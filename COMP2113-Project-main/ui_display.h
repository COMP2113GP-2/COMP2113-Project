/*
 * ui_display.h
 * Role 3: ASCII Ship Display & Animation System
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey
 */

#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H

#include "game.h"
#include "event.h"
#include <string>

namespace Display {

    /*
     * Prints the animated title banner.
     * Input:  none
     * Output: ASCII title to stdout
     */
    void showTitleBanner();

    /*
     * Draws the full game HUD with ship art, resource bars, and progress track.
     * Input:  game — current game state
     *         sf   — sanity/fatigue state
     * Output: HUD printed to stdout
     */
    void showGameHUD(const GameState& game, const SanityFatigue& sf,
                     const std::string& lastOutcome = "");

    /*
     * Sailing animation — side-view ship crossing waves.
     * Input:  distanceGained — shown during animation
     *         days           — number of days sailed
     * Output: animation frames to stdout
     */
    void sailAnimation(int distanceGained, int days);

    /*
     * Exploration animation — telescope scanning horizon.
     * Input:  none
     * Output: animation frames to stdout
     */
    void exploreAnimation();

    /*
     * Storm animation — lightning and churning waves.
     * Input:  none
     * Output: animation frames to stdout
     */
    void stormAnimation();

    /*
     * Rest animation — bobbing anchor and gentle waves.
     * Input:  none
     * Output: animation frames to stdout
     */
    void restAnimation();

    /*
     * Colored event notification banner.
     * Input:  eventName — name of the event
     *         isGood    — true = cyan, false = red
     * Output: banner to stdout
     */
    void showEventBanner(const std::string& eventName, bool isGood);

    /*
     * Port arrival banner.
     * Input:  portKm — distance at which this port appears
     * Output: banner to stdout
     */
    void showPortBanner(int portKm);

    /*
     * Striking red banner shown when the player's pet dies.
     * Input:  petName — name of the deceased pet
     * Output: banner to stdout
     */
    void showPetDeathNotice(const std::string& petName);

    /*
     * Victory screen with waving flag animation.
     * Input:  daysUsed — total days of voyage
     *         bff      — whether BFF achievement was earned
     * Output: animation to stdout
     */
    void victoryScreen(int daysUsed, bool bff);

    /*
     * Defeat screen with sinking ship animation.
     * Input:  reason — why the player lost
     * Output: animation to stdout
     */
    void defeatScreen(const std::string& reason);

} // namespace Display

#endif // UI_DISPLAY_H