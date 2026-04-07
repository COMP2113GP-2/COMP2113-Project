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

// ============================================================================
// EVENT TABLES
// ============================================================================

// ---------- Deterministic good events (fixed reward type) ----------
static const int DET_COUNT=4;

static const char* DET_NAMES[DET_COUNT] = {
    "Rainfall Replenishment",
    "Drifting Timber",
    "Favorable Current",
    "Sunrise at Sea"
};
static const char* DET_DESCS[DET_COUNT] = {
    "Rain clouds roll in -- the crew scrambles to collect precious fresh water.",
    "Sturdy drifting timber is hauled aboard -- just what the hull needs.",
    "A strong current grabs the hull and surges you forward with ease.",
    "The horizon blazes gold at dawn -- spirits soar across the whole crew."
};

// ---------- Exploratory good events (randomised 2-4 rewards) ----------
static const int EXP_COUNT=4;

static const char* EXP_NAMES[EXP_COUNT] = {
    "Encounter Fishing Boat",
    "Find Drifting Crate",
    "Rescue Merchant Ship",
    "Discover Fish School"
};
static const char* EXP_DESCS[EXP_COUNT] = {
    "A friendly fishing boat crosses your path and shares their catch generously.",
    "A waterlogged crate bobs alongside the hull, still sealed and full of supplies.",
    "You answer a distress signal; the grateful merchant rewards you handsomely.",
    "The lookout spots a massive school of fish. Lines go in immediately."
};

// ---------- Bad events ----------
static const int BAD_COUNT=5;

struct BadEventTemplate {
    const char* name;
    const char* description;
    const char* choiceALabel;
    const char* choiceBLabel;
    const char* choiceADetail;
    const char* choiceBDetail;
};

static const BadEventTemplate BAD_EVENTS[BAD_COUNT] = {
    {
        "Sudden Storm",
        "Black clouds rear up without warning. Waves hammer the hull and "
        "lightning splits the sky.",
        "Slow down to avoid",
        "Continue sailing through",
        "Reduce speed and weather it out.   [Durability -1,  1 day lost]",
        "Push straight through the gale.    [Durability -3,  Water -1]"
    },
    {
        "Crew Seasickness",
        "Half the crew turns green -- moaning, retching, unable to keep watch.",
        "Force rest for recovery",
        "Continue sailing shorthanded",
        "Heave-to and let the crew recover.  [Stamina +2,  1 day lost]",
        "Sail on with a skeleton crew.       [Stamina -1]"
    },
    {
        "Reef Collision",
        "A grinding scrape from below -- you have clipped a hidden reef. "
        "Water seeps in slowly.",
        "Emergency repair",
        "Push forward and ignore it",
        "Patch the hull properly.            [Gold -30,  Durability -1]",
        "Sail on with the damage.            [Durability -3,  distance halved]"
    },
    {
        "Supplies Lost Overboard",
        "A rogue wave sweeps the deck -- crates and barrels vanish into the sea.",
        "Retrieve what you can",
        "Cut your losses and sail on",
        "Send crew over the side.            [1 day lost,  lose 1-2 random supplies]",
        "Leave it and keep speed.            [lose 2-4 random supplies]"
    },
    {
        "Lost Direction",
        "The compass spins, stars are hidden, and no one agrees on the heading.",
        "Careful detour",
        "Trust your gut",
        "Navigate a known detour.            [2 days lost]",
        "Rely on experience.                 [1 day lost,  Sanity -5]"
    }
};

// ============================================================================
// INTERNAL: BUILD GOOD EVENT
// ============================================================================

/*
 * Hybrid good event builder.
 *
 * 50% chance: Deterministic event (Rainfall/Timber/Current/Sunrise)
 *   -> fixed reward type, amount scales with difficulty
 *
 * 50% chance: Exploratory event (Fishing Boat/Crate/Merchant/Fish School)
 *   -> 2-4 randomised rewards from loot pool, no duplicates allowed
 *
 * Loot pool for exploratory events (6 types):
 *   0=food, 1=water, 2=gold, 3=durability, 4=sanity, 5=distance
 */
static EventResult buildGoodEvent(Difficulty difficulty) {
    EventResult r;
    r.eventOccurred=true;
    r.isGoodEvent= true;

    bool isDeterministic = (randRange(0, 1) == 0);

    // ------------------------------------------------------------------
    // BRANCH A: DETERMINISTIC
    // ------------------------------------------------------------------
    if (isDeterministic) {
        int idx    = randRange(0, DET_COUNT - 1);
        r.eventName = DET_NAMES[idx];

        int distBonus = 0;

        switch (idx) {
            case 0: // Rainfall Replenishment -> Fresh Water
                switch (difficulty) {
                    case Difficulty::STORY:  r.waterDelta = 4; break;
                    case Difficulty::EASY:   r.waterDelta = 3; break;
                    case Difficulty::MEDIUM: r.waterDelta = 2; break;
                    case Difficulty::HARD:   r.waterDelta = 1; break;
                    default:                 r.waterDelta = 2; break;
                }
                break;

            case 1: // Drifting Timber -> Ship Durability (+1 always)
                r.durabilityDelta = 1;
                break;

            case 2: // Favorable Current -> Distance bonus
                switch (difficulty) {
                    case Difficulty::STORY:  distBonus = 30; break;
                    case Difficulty::EASY:   distBonus = 25; break;
                    case Difficulty::MEDIUM: distBonus = 20; break;
                    case Difficulty::HARD:   distBonus = 15; break;
                    default:                 distBonus = 20; break;
                }
                r.extraDaysLost = -distBonus; // negative = km bonus for main.cpp
                break;

            case 3: // Sunrise at Sea -> Sanity
                switch (difficulty) {
                    case Difficulty::STORY:  r.sanityDelta = 8; break;
                    case Difficulty::EASY:   r.sanityDelta = 5; break;
                    case Difficulty::MEDIUM: r.sanityDelta = 3; break;
                    case Difficulty::HARD:   r.sanityDelta = 3; break;
                    default:                 r.sanityDelta = 5; break;
                }
                break;
        }

        // Build gain summary
        string gain = "";
        auto ap = [&](const string& s) {
            if (!gain.empty()) gain += ", ";
            gain += s;
        };
        if (r.waterDelta > 0)
            ap("Water +"      + to_string(r.waterDelta));
        if (r.durabilityDelta > 0)
            ap("Durability +" + to_string(r.durabilityDelta));
        if (distBonus > 0)
            ap("Distance +"   + to_string(distBonus) + " km");
        if (r.sanityDelta > 0)
            ap("Sanity +"     + to_string(r.sanityDelta));

        r.description = "You encounter " + r.eventName + "!\n" +
                        string(DET_DESCS[idx]) + "\n" +
                        "You gain: " + gain + ".";
        return r;
    }

    // ------------------------------------------------------------------
    // BRANCH B: EXPLORATORY — 2 to 4 rewards
    // ------------------------------------------------------------------
    int idx    = randRange(0, EXP_COUNT - 1);
    r.eventName = EXP_NAMES[idx];

    int rewardCount = randRange(2, 4);

    // Loot pool: 0=food 1=water 2=gold 3=durability 4=sanity 5=distance
    bool selected[6] = { false, false, false, false, false, false };
    int  distBonus   = 0;

    // Applies one loot type to r, accumulates distBonus
    auto applyLoot = [&](int lootType) {
        switch (lootType) {
            case 0: // food
                switch (difficulty) {
                    case Difficulty::STORY:  r.foodDelta += 3;               break;
                    case Difficulty::EASY:   r.foodDelta += 2;               break;
                    case Difficulty::MEDIUM: r.foodDelta += 2;               break;
                    case Difficulty::HARD:   r.foodDelta += randRange(1, 2); break;
                    default:                 r.foodDelta += 2;               break;
                }
                break;
            case 1: // water
                switch (difficulty) {
                    case Difficulty::STORY:  r.waterDelta += 3;              break;
                    case Difficulty::EASY:   r.waterDelta += 2;              break;
                    case Difficulty::MEDIUM: r.waterDelta += 2;              break;
                    case Difficulty::HARD:   r.waterDelta += randRange(1,2); break;
                    default:                 r.waterDelta += 2;              break;
                }
                break;
            case 2: // gold
                switch (difficulty) {
                    case Difficulty::STORY:  r.goldDelta += randRange(50,80); break;
                    case Difficulty::EASY:   r.goldDelta += randRange(40,60); break;
                    case Difficulty::MEDIUM: r.goldDelta += randRange(30,50); break;
                    case Difficulty::HARD:   r.goldDelta += randRange(20,30); break;
                    default:                 r.goldDelta += randRange(30,50); break;
                }
                break;
            case 3: // durability
                r.durabilityDelta += 1;
                break;
            case 4: // sanity
                switch (difficulty) {
                    case Difficulty::STORY:  r.sanityDelta += 5; break;
                    case Difficulty::EASY:   r.sanityDelta += 5; break;
                    case Difficulty::MEDIUM: r.sanityDelta += 3; break;
                    case Difficulty::HARD:   r.sanityDelta += 3; break;
                    default:                 r.sanityDelta += 3; break;
                }
                break;
            case 5: // distance bonus
                switch (difficulty) {
                    case Difficulty::STORY:  distBonus += 30; break;
                    case Difficulty::EASY:   distBonus += 25; break;
                    case Difficulty::MEDIUM: distBonus += 20; break;
                    case Difficulty::HARD:   distBonus += 15; break;
                    default:                 distBonus += 20; break;
                }
                break;
        }
    };

    for (int i = 0; i < rewardCount; i++) {
        // Pick an unused loot type; retry up to 20 times
        int lootType = -1;
        for (int attempt = 0; attempt < 20; attempt++) {
            int candidate = randRange(0, 5);
            if (!selected[candidate]) {
                lootType = candidate;
                selected[candidate] = true;
                break;
            }
        }
        if (lootType == -1) break; // all 6 types used (rewardCount was 4+)
        applyLoot(lootType);
    }

    if (distBonus > 0)
        r.extraDaysLost = -distBonus;

    // Build gain summary
    string gain = "";
    auto ap = [&](const string& s) {
        if (!gain.empty()) gain += ", ";
        gain += s;
    };
    if (r.foodDelta > 0)
        ap("Food +"       + to_string(r.foodDelta));
    if (r.waterDelta > 0)
        ap("Water +"      + to_string(r.waterDelta));
    if (r.goldDelta > 0)
        ap("Gold +"       + to_string(r.goldDelta));
    if (r.durabilityDelta > 0)
        ap("Durability +" + to_string(r.durabilityDelta));
    if (r.sanityDelta > 0)
        ap("Sanity +"     + to_string(r.sanityDelta));
    if (distBonus > 0)
        ap("Distance +"   + to_string(distBonus) + " km");

    r.description = "You encounter " + r.eventName + "!\n" +
                    string(EXP_DESCS[idx]) + "\n" +
                    "You gain: " + gain + ".";
    return r;
}

// ============================================================================
// INTERNAL: BUILD BAD EVENT
// ============================================================================

/*
 * Picks a random bad event and fills choice text.
 * Deltas are zeroed — resolvePlayerChoice() sets them after player decides.
 */
static EventResult buildBadEvent() {
    EventResult r;
    r.eventOccurred = true;
    r.isGoodEvent   = false;

    int idx = randRange(0, BAD_COUNT - 1);
    const BadEventTemplate& t = BAD_EVENTS[idx];

    r.eventName     = t.name;
    r.choiceALabel  = t.choiceALabel;
    r.choiceBLabel  = t.choiceBLabel;
    r.choiceADetail = t.choiceADetail;
    r.choiceBDetail = t.choiceBDetail;

    r.description = "You encounter " + r.eventName + "!\n" +
                    string(t.description) + "\n" +
                    "What do you do?\n" +
                    "  [1] " + string(t.choiceADetail) + "\n" +
                    "  [2] " + string(t.choiceBDetail);
    return r;
}

