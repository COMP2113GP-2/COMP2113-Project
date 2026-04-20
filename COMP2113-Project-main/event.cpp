/*
 * event.cpp
 * Role 3: Random Event System + Difficulty + Loot Tables
 * Course: COMP2113
 * Project: ABYSSAL ODYSSEY
 */

#include "event.h"
#include "ui_display.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <limits>  // for numeric_limits

static const std::string FG_GOLD = "\033[33m";
static const std::string BOLD    = "\033[1m";
static const std::string RST     = "\033[0m";

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
 * Input: difficulty enum
 * Output: filled GameDifficultySettings
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
 * 
 * Input: difficulty
 * Output: fully filled EventResult
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
 * Input: none
 * Output: EventResult with choice text filled, deltas zeroed
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

// ============================================================================
// INTERNAL: RESOLVE PLAYER CHOICE (bad events)
// ============================================================================

/*
 * Fills resource deltas into result based on player's choice.
 * Input: EventResult from buildBadEvent()
 *         choice — 1= choice A (safer), 2 =choice B (riskier)
 * Output: result deltas filled, description updated with outcome
 */
static void resolvePlayerChoice(EventResult& result, int choice) {
    // Match event name to template index
    int idx = 0;
    for (int i = 0; i < BAD_COUNT; i++) {
        if (result.eventName == BAD_EVENTS[i].name) {
            idx = i;
            break;
        }
    }

    // 0 = Sudden Storm
    // 1 = Crew Seasickness
    // 2 = Reef Collision
    // 3 = Supplies Lost Overboard
    // 4 = Lost Direction

    if (choice == 1) {
        // ---- CHOICE A: Safer ----
        switch (idx) {
            case 0: // Sudden Storm: slow down
                result.durabilityDelta = -1;
                result.extraDaysLost   =  1;
                break;
            case 1: // Seasickness: force rest
                result.staminaDelta  =  2;
                result.extraDaysLost =  1;
                break;
            case 2: // Reef: emergency repair
                result.goldDelta       = -30;
                result.durabilityDelta = -1;
                break;
            case 3: // Supplies lost: retrieve
                result.extraDaysLost = 1;
                if (randRange(0, 1) == 0)
                    result.foodDelta  = -randRange(1, 2);
                else
                    result.waterDelta = -randRange(1, 2);
                break;
            case 4: // Lost direction: careful detour
                result.extraDaysLost = 2;
                break;
            default: break;
        }
    } else {
        // ---- CHOICE B: Riskier ----
        switch (idx) {
            case 0: // Sudden Storm: push through
                result.durabilityDelta = -3;
                result.waterDelta      = -1;
                break;
            case 1: // Seasickness: sail shorthanded
                result.staminaDelta = -1;
                break;
            case 2: // Reef: push forward
                result.durabilityDelta     = -3;
                result.halfDistanceThisTurn = true;
                break;
            case 3: // Supplies lost: abandon
                result.foodDelta  = -randRange(1, 2);
                result.waterDelta = -randRange(1, 2);
                break;
            case 4: // Lost direction: trust gut
                result.extraDaysLost = 1;
                result.sanityDelta   = -5;
                break;
            default: break;
        }
    }

    // Append outcome summary to description
    string summary = "";
    auto ap = [&](const string& s) {
        if (!summary.empty()) summary += ", ";
        summary += s;
    };
    if (result.foodDelta != 0)
        ap("Food "       + to_string(result.foodDelta));
    if (result.waterDelta != 0)
        ap("Water "      + to_string(result.waterDelta));
    if (result.goldDelta != 0)
        ap("Gold "       + to_string(result.goldDelta));
    if (result.durabilityDelta != 0)
        ap("Durability " + to_string(result.durabilityDelta));
    if (result.staminaDelta != 0)
        ap("Stamina "    + to_string(result.staminaDelta));
    if (result.sanityDelta != 0)
        ap("Sanity "     + to_string(result.sanityDelta));
    if (result.extraDaysLost > 0)
        ap(to_string(result.extraDaysLost) + " day(s) lost");
    if (result.halfDistanceThisTurn)
        ap("distance this turn halved");

    if (!summary.empty())
        result.description += "\nOutcome: " + summary + ".";
}

// ============================================================================
// EVENT NAMESPACE — public API matching main.cpp calling convention
// ============================================================================

namespace Event {

// ----------------------------------------------------------------------------
// initSanityFatigue
// ----------------------------------------------------------------------------

/*
 * Returns a zero-initialised SanityFatigue for the given difficulty.
 * Input: difficulty — chosen by player
 * Output: initialised SanityFatigue
 */
SanityFatigue initSanityFatigue(Difficulty difficulty) {
    (void)difficulty;
    SanityFatigue sf;
    sf.consecutiveSailingDays = 0;
    sf.extraBadEventChance    = 0;
    return sf;
}

// ----------------------------------------------------------------------------
// updateFatigue
// ----------------------------------------------------------------------------

/*
 * Updates consecutive sailing day counter after an action.
 * Sail (1): increments counter.
 * Rest (2) or Explore (3): resets counter to 0.
 *
 * Input:  sf     — modified in place
 *         action — 1=Sail, 2=Rest, 3=Explore
 * Output: sf.consecutiveSailingDays updated
 */
void updateFatigue(SanityFatigue& sf, int action) {
    if (action == 1) {
        sf.consecutiveSailingDays++;
    } else {
        sf.consecutiveSailingDays = 0;
    }
}

// ----------------------------------------------------------------------------
// updateSanity
// ----------------------------------------------------------------------------

/*
 * Recalculates sf.extraBadEventChance from current game state.
 *   +5  if crew sanity is CRITICAL (0-39)
 *   +20 if consecutiveSailingDays >= 5 (fatigue)
 * Input: sf   — modified in place
 *        game — reads getSanityStage()
 * Output: sf.extraBadEventChance updated
 */
void updateSanity(SanityFatigue& sf, const Game& game) {
    int extra = 0;
    if (game.getSanityStage() == SanityStage::CRITICAL) {
        extra += 5;
    }
    if (sf.consecutiveSailingDays >= 5) {
        extra += 20;
    }
    sf.extraBadEventChance = extra;
}

// ----------------------------------------------------------------------------
// triggerRandomEvent
// ----------------------------------------------------------------------------

/*
 * Rolls against (base difficulty chance + sf.extraBadEventChance).
 * Input:  game — reads difficulty from game state
 *        sf   — reads extraBadEventChance
 * Output: true if an event fires this turn
 */
bool triggerRandomEvent(const Game& game, const SanityFatigue& sf) {
    GameDifficultySettings s =
        getDifficultySettings(game.getGameState().difficulty);

    int chance = s.eventChancePercent + sf.extraBadEventChance;
    if (chance > 95) chance = 95;

    int roll = randRange(1, 100);
    return roll <= chance;
}

// ----------------------------------------------------------------------------
// runRandomEvent
// ----------------------------------------------------------------------------

/*
 * Full event flow — called by main.cpp after triggerRandomEvent() is true.
 * Input: game — modified in place
 *        sf — reads difficulty for loot tables
 * Output: void
 */
void runRandomEvent(Game& game, const SanityFatigue& sf) {
    runRandomEventWithResult(game, sf);
}

EventResult runRandomEventWithResult(Game& game, const SanityFatigue& sf) {
    GameDifficultySettings s =
        getDifficultySettings(game.getGameState().difficulty);

    int roll = randRange(1, 100);
    EventResult result;

    if (roll <= s.goodEventWeight) {
        result = buildGoodEvent(game.getGameState().difficulty);
    } else {
        result = buildBadEvent();
    }

    // Show banner
    Display::showEventBanner(result.eventName, result.isGoodEvent);

    // Play storm animation specifically for Sudden Storm event
    if (result.eventName == "Sudden Storm") {
        Display::stormAnimation();
    }

    if (!result.isGoodEvent) {
        cout << result.description << "\n";

        int choice = 0;
        while (choice != 1 && choice != 2) {
            cout << "\nEnter your choice (1 or 2): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (choice != 1 && choice != 2)
                cout << "Invalid input. Please enter 1 or 2.\n";
        }
        resolvePlayerChoice(result, choice);
        
        // Extract just the outcome part from description
        size_t pos = result.description.rfind("\nOutcome: ");
        cout << "\n" << FG_GOLD + BOLD + "  Outcome: " + RST;
        if (pos != string::npos)
            cout << result.description.substr(pos + 10) << "\n";
        else
            cout << "(no change)\n";
    } else {
        // Good event — show gains
        cout << result.description << "\n";
    }

    cout << FG_GOLD + "  ========================================\n" + RST;

    game.applyEventEffects(
        result.foodDelta,
        result.waterDelta,
        result.goldDelta,
        result.sanityDelta,
        result.staminaDelta,
        result.durabilityDelta
    );

    return result;
}

} // namespace Event