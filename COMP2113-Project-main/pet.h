#ifndef PET_H
#define PET_H

#include "game.h"
#include "event.h"
#include <string>

// ============================================================================
// PET TYPE ENUM
// ============================================================================

enum class PetType {
    CAT,
    DOG,
    PARROT
};

// ============================================================================
// PET STATE STRUCT (Extended pet info beyond game.h's Pet struct)
// ============================================================================

struct PetState {
    std::string name;
    PetType type;
    bool alive;
    int daysAsCompanion;
    bool bffUnlocked;
    bool hasPet;

    PetState()
        : name(""), type(PetType::CAT), alive(true),
          daysAsCompanion(0), bffUnlocked(false), hasPet(false) {}
};

// Global pet state — main.cpp declares: extern PetState g_pet;
extern PetState g_pet;

// ============================================================================
// PET NAMESPACE
// ============================================================================

namespace PetSystem {

    // ========================================================================
    // CORE PET FUNCTIONS (User Spec)
    // ========================================================================

    /**
     * Prompts player to pick Cat/Dog/Parrot and name the pet.
     * @return Fully initialized PetState
     */
    PetState createPet();

    /**
     * Daily pet update: food -10, mood -5. Pet dies if both hit 0.
     * Increments daysAsCompanion and checks BFF achievement.
     */
    void updatePetDaily(GameState& game);

    /**
     * Feeds the pet: food +25, mood +5 (clamped 0-100).
     */
    void feedPet(GameState& game);

    /**
     * Play with the pet: mood +15, prints type-specific flavor text.
     */
    void interactWithPet(GameState& game);

    /**
     * Unlocks BFF achievement if pet alive 10+ days with mood > 50.
     */
    void checkBFF(const GameState& game);

    /**
     * Prints all pet stats to console.
     */
    void displayPetStatus(const GameState& game);

    /**
     * @return true if pet is alive and mood > 60
     */
    bool petMoraleBonus(const GameState& game);

    // ========================================================================
    // WRAPPER FUNCTIONS (Match main.cpp calling convention)
    // ========================================================================

    /**
     * Asks player if they want a pet, creates one if yes.
     * Populates the global g_pet.
     */
    void initPet();

    /**
     * Called each turn from main loop. Runs updatePetDaily.
     * @param game Current game state
     * @param action 1=Sail, 2=Rest, 3=Explore
     */
    void updatePet(GameState& game, int action);

    /**
     * Shows pet interaction menu (Feed/Play/Status/Back).
     * Applies crew sanity bonus if pet morale is high.
     */
    void interactPet(GameState& game, SanityFatigue& sf);

} // namespace PetSystem

#endif // PET_H
