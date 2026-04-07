/*
 * event.cpp
 * Role 3: Random Event System + Difficulty + Loot Tables
 * Course: COMP2113
 * Project: Ocean Voyage
 */

#include "event.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

// ============================================================================
// INTERNAL HELPER
// ============================================================================

/*
 * Returns a random int in [lo, hi] inclusive.
 * Relies on srand() having been called once in main.cpp.
 *
 * Input:  lo, hi — inclusive range bounds
 * Output: random int in [lo, hi]
 */
static int randRange(int lo, int hi) {
    if (lo >= hi) return lo;
    return lo + rand() % (hi - lo + 1);
}

// ============================================================================
// CENTRAL DIFFICULTY SETTINGS
// ============================================================================

/*
 * Returns complete difficulty settings for the given difficulty.
 *
 * Difficulty table:
 * ---------------------------------------------------------------
 *              STORY   EASY   MEDIUM   HARD
 * daysLimit      -1     92      61      41
 * eventChance%   20     30      40      50
 * priceMulti    0.8    1.0     1.2     1.5
 * sanityDrain    -1     -1      -2      -3
 * goodWeight     70     60      50      35
 * badWeight      30     40      50      65
 * portEveryKm   500    500     700    1000
 * ---------------------------------------------------------------
 */
GameDifficultySettings getDifficultySettings(Difficulty difficulty) {
    GameDifficultySettings s;
    switch (difficulty) {
        case Difficulty::STORY:
            s.daysLimit          = -1;
            s.eventChancePercent = 20;
            s.priceMultiplier    = 0.8;
            s.sanityDrainPerDay  = -1;
            s.goodEventWeight    = 70;
            s.badEventWeight     = 30;
            s.portEveryKm        = 500;
            break;
        case Difficulty::EASY:
            s.daysLimit          = 92;
            s.eventChancePercent = 30;
            s.priceMultiplier    = 1.0;
            s.sanityDrainPerDay  = -1;
            s.goodEventWeight    = 60;
            s.badEventWeight     = 40;
            s.portEveryKm        = 500;
            break;
        case Difficulty::MEDIUM:
            s.daysLimit          = 61;
            s.eventChancePercent = 40;
            s.priceMultiplier    = 1.2;
            s.sanityDrainPerDay  = -2;
            s.goodEventWeight    = 50;
            s.badEventWeight     = 50;
            s.portEveryKm        = 700;
            break;
        case Difficulty::HARD:
            s.daysLimit          = 41;
            s.eventChancePercent = 50;
            s.priceMultiplier    = 1.5;
            s.sanityDrainPerDay  = -3;
            s.goodEventWeight    = 35;
            s.badEventWeight     = 65;
            s.portEveryKm        = 1000;
            break;
        default:
            s.daysLimit          = 92;
            s.eventChancePercent = 30;
            s.priceMultiplier    = 1.0;
            s.sanityDrainPerDay  = -1;
            s.goodEventWeight    = 60;
            s.badEventWeight     = 40;
            s.portEveryKm        = 500;
            break;
    }
    return s;
}

