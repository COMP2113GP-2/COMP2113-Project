/*
 * event.h
 * Role 3: Random Event System + Difficulty + Loot Tables
 * Course: COMP2113
 * Project: ABYSSAL ODYSSEY
 *
 * Declares:
 *  - GameDifficultySettings : central difficulty table
 *  - getDifficultySettings(): free function, all modules read difficulty numbers here
 *  - SanityFatigue          : tracks consecutive sailing days and sanity stage
 *  - EventResult            : full event data returned to main/UI
 *  - Event namespace        : static functions matching main.cpp calling convention
 */

#ifndef EVENT_H
#define EVENT_H

#include "game.h"
#include <string>

// ============================================================================
// CENTRAL DIFFICULTY SETTINGS
// Who reads what:
//   game.cpp  -> daysLimit, sanityDrainPerDay, priceMultiplier
//   port.cpp  -> priceMultiplier, portEveryKm
//   event.cpp -> eventChancePercent, goodEventWeight, badEventWeight
// ============================================================================

struct GameDifficultySettings {
    int    daysLimit;           // -1=unlimited/92/61/41
    int    eventChancePercent;  // Base % an event fires after Sail or Explore
    double priceMultiplier;     // Port price multiplier: 0.8/1.0/1.2/1.5
    int    sanityDrainPerDay;   // Negative value: -1/-1/-2/-3
    int    goodEventWeight;     // Out of 100: chance triggered event is good
    int    badEventWeight;      // Out of 100: chance triggered event is bad
    int    portEveryKm;         // Port spacing: 500/500/700/1000km
};

GameDifficultySettings getDifficultySettings(Difficulty difficulty);


// ============================================================================
// SANITY FATIGUE STRUCT
// Tracks consecutive sailing days and current event probability modifiers.
// Lives in main.cpp as: extern SanityFatigue g_sf
// ============================================================================

struct SanityFatigue {
    int consecutiveSailingDays; // Resets to 0 after Rest action
    int extraBadEventChance;    // Added on top of base difficulty chance
                                // +5 for critical sanity, +20 for fatigue

    SanityFatigue()
        : consecutiveSailingDays(0), extraBadEventChance(0) {}
};


// ============================================================================
// EVENT RESULT STRUCT
// Filled by Event::runRandomEvent() and passed to UI for display.
// Resource deltas are applied to game via game.applyEventEffects().
// ============================================================================

struct EventResult {
    bool        eventOccurred;        // false = no event triggered this turn
    bool        isGoodEvent;          // true = good event, false = bad event

    std::string eventName;
    std::string description;          // Full player-facing message with gains/losses

    // Bad events only: displayed before player chooses
    std::string choiceALabel;         // Short label e.g. "Slow down to avoid"
    std::string choiceBLabel;         // Short label e.g. "Continue sailing through"
    std::string choiceADetail;        // Full detail for choice A shown to player
    std::string choiceBDetail;        // Full detail for choice B shown to player

    // Resource deltas—zeroed until resolvePlayerChoice() for bad events
    int  foodDelta;
    int  waterDelta;
    int  goldDelta;
    int  sanityDelta;
    int  staminaDelta;
    int  durabilityDelta;
    int  extraDaysLost;               // main adds this to currentDay (if > 0)
                                      // NEGATIVE = distance bonus km (good events)
    bool halfDistanceThisTurn;        // true = Reef choice B: main halves distance

    // Constructor: zero-initialise everything
    EventResult()
        : eventOccurred(false), isGoodEvent(false),
          foodDelta(0), waterDelta(0), goldDelta(0),
          sanityDelta(0), staminaDelta(0), durabilityDelta(0),
          extraDaysLost(0), halfDistanceThisTurn(false) {}
};


// ============================================================================
// EVENT NAMESPACE
// All functions are free functions inside the Event namespace so main.cpp
// can call them as Event::triggerRandomEvent(game, g_sf) etc.
// ============================================================================

namespace Event {

    /*
     * Initialises a SanityFatigue struct for the chosen difficulty.
     * Call once at game start (new game or after load).
     * Input:  difficulty — chosen by player
     * Output: initialised SanityFatigue struct
     */
    SanityFatigue initSanityFatigue(Difficulty difficulty);

    /*
     * Updates g_sf after the player takes an action this turn.
     * Increments consecutiveSailingDays for Sail, resets for Rest/Explore.
     *         action — 1 = Sail, 2 = Rest, 3 = Explore
     * Input:  sf     — SanityFatigue to update (modified in place)
     *         action — 1 = Sail, 2 = Rest, 3 = Explore
     * Output: sf.consecutiveSailingDays updated
     */
    void updateFatigue(SanityFatigue& sf, int action);

    /*
     * Recalculates sf.extraBadEventChance based on current game state.
     * +5 if sanity is CRITICAL, +20 if fatigued (5+ consecutive days).
     *
     * Input:  sf—SanityFatigue to update (modified in place)
     *         game— current Game object (reads sanity stage)
     * Output: sf.extraBadEventChance updated
     */
    void updateSanity(SanityFatigue& sf, const Game& game);

    /*
     * Decides whether a random event triggers this turn.
     * Uses base difficulty chance + sf.extraBadEventChance.
     *
     * Input:  game —reads difficulty from game state
     *         sf — reads extraBadEventChance
     * Output: true if an event fires this turn
     */
    bool triggerRandomEvent(const Game& game, const SanityFatigue& sf);

    /*
     * Generates a full EventResult (good or bad), displays it via cout,
     * prompts player choice for bad events, applies deltas to game,
     * and handles extraDaysLost and distance bonuses.
     *
     * This is the single entry point for the entire event flow.
     * main.cpp calls this after triggerRandomEvent() returns true.
     *
     * Input:  game— modified in place via game.applyEventEffects()
     *         sf — reads difficulty for loot table selection
     * Output: void (all effects applied directly to game)
     */
    void runRandomEvent(Game& game, const SanityFatigue& sf);

    /*
     * extraDaysLost and distance bonuses.
     * Input:  game, sf
     * Output: EventResult with all deltas filled
     */
    EventResult runRandomEventWithResult(Game& game, const SanityFatigue& sf);

} // namespace Event

#endif // EVENT_H