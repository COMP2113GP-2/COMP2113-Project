#ifndef GAME_H
#define GAME_H

#include <string>

// ============================================================================
// ENUMS
// ============================================================================

enum class Difficulty {
    STORY,
    EASY,
    MEDIUM,
    HARD
};

enum class GameStatus {
    ONGOING,
    VICTORY,
    DEFEAT
};

enum class SanityStage {
    CRITICAL,  // 0-39: Increased bad event chance
    NORMAL,    // 40-79: Normal
    GOOD       // 80-100: Extra action available
};

// ============================================================================
// DIFFICULTY CONFIGURATION STRUCT
// ============================================================================

struct DifficultyConfig {
    int daysLimit;           // Unlimited (-1), 92, 61, 41
    int sanityDrainPerDay;   // -1, -1, -2, -3
    double priceMultiplier;  // 0.8, 1.0, 1.2, 1.5
};

// ============================================================================
// RESOURCE STRUCT
// ============================================================================

struct Resource {
    int food;        // No cap. Initial: 20
    int freshWater;  // No cap. Initial: 20
    int gold;        // No cap. Initial: 150
    
    // FIX: Added constructor to initialize starting values correctly
    Resource() : food(20), freshWater(20), gold(150) {}
};

// ============================================================================
// PET STRUCT (Optional)
// ============================================================================

struct Pet {
    int petFood;     // No cap. Initial: 10
    int petMood;     // Cap: 100. Initial: 70
    
    Pet() : petFood(10), petMood(70) {}
};

// ============================================================================
// CREW STRUCT
// ============================================================================

struct Crew {
    int stamina;     // Cap: 10. Initial: 10. Lose if 0.
    int sanity;      // Cap: 100. Initial: 70
    
    Crew() : stamina(10), sanity(70) {}
};

// ============================================================================
// SHIP STRUCT
// ============================================================================

struct Ship {
    int distance;    // Current distance traveled. Initial: 0. Target: 3000 km
    int durability;  // Cap: 10. Initial: 10. Lose if 0.
    
    Ship() : distance(0), durability(10) {}
};

// ============================================================================
// GAME STATE STRUCT
// ============================================================================

struct GameState {
    Ship ship;
    Crew crew;
    Resource resources;
    Pet* pet;                    // Dynamic memory: nullptr if no pet
    
    int currentDay;              
    int consecutiveSailingDays;  
    int daysWithoutWaterResupply; 
    
    Difficulty difficulty;
    GameStatus status;
    
    GameState(Difficulty diff, bool hasPet) 
        : ship(), crew(), resources(), pet(nullptr), 
          currentDay(0), consecutiveSailingDays(0), 
          daysWithoutWaterResupply(0), difficulty(diff), 
          status(GameStatus::ONGOING) {
        if (hasPet) {
            pet = new Pet();
        }
    }
    
    ~GameState() {
        if (pet != nullptr) {
            delete pet;
            pet = nullptr;
        }
    }

    // FIX (C++11): Prevent copying to avoid Double-Free memory crashes with pet pointer
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;
};

// ============================================================================
// ACTION RESULT STRUCT (For UI/Event System Feedback)
// ============================================================================

struct ActionResult {
    int daysSpent;              
    int distanceGained;         
    
    int foodDelta;
    int waterDelta;
    int goldDelta;
    int staminaDelta;
    int sanityDelta;            
    int durabilityDelta;
    int petFoodDelta;           
    int petMoodDelta;           
    
    std::string actionName;
};

// ============================================================================
// CORE GAME LOGIC CLASS
// ============================================================================

class Game {
private:
    GameState gameState;
    DifficultyConfig difficultyConfig;
    
    // ========================================================================
    // PRIVATE HELPER METHODS
    // ========================================================================
    
    /**
     * Applies difficulty-specific sanity drain for days_passed.
     * @param daysSpent: Number of days to apply drain for
     */
    void applySanityDrain(int daysSpent);
    
    /**
     * Applies fatigue penalty if consecutive sailing days >= 5.
     */
    void applyFatiguePenalty();
    
    /**
     * Checks and updates daysWithoutWaterResupply counter.
     */
    void updateWaterResupplyCounter();
    
    /**
     * Validates if a given action is possible given current state.
     */
    bool canPerformAction() const;
    
    /**
     * Comprehensive check for win/lose conditions.
     */
    void checkWinLoseConditions();
    
    /**
     * Helper: Clamps a value to a range [min, max]
     */
    int clamp(int value, int min, int max) const;
    
public:
    // ========================================================================
    // CONSTRUCTOR
    // ========================================================================
    
    /**
     * Initializes game with selected difficulty and pet choice.
     * @param difficulty: Story, Easy, Medium, or Hard
     * @param hasPet: true to initialize pet system, false otherwise
     */
    Game(Difficulty difficulty, bool hasPet);
    
    // ========================================================================
    // ACTION METHODS (Called by Role 5)
    // ========================================================================
    
    /**
     * SAIL ACTION: Travel towards destination.
     * @param daysToBeSailed: 2-4 (from UI input)
     * @return ActionResult with all resource/stat changes
     */
    ActionResult sail(int daysToBeSailed);
    
    /**
     * REST ACTION: Recover crew stamina and boost morale.
     * @param daysToRest: 1-3 (from UI input)
     * @return ActionResult with all resource/stat changes
     */
    ActionResult rest(int daysToRest);
    
    /**
     * EXPLORE ACTION: Search for resources/loot.
     * @param daysToExplore: 1-2 (from UI input)
     * @return ActionResult with base consumption only
     */
    ActionResult explore(int daysToExplore);
    
    // ========================================================================
    // GAME STATE QUERY METHODS (Called by Roles 3, 4, 5)
    // ========================================================================
    
    /**
     * Returns complete current game state snapshot.
     * @return const reference to GameState
     */
    const GameState& getGameState() const;
    
    /**
     * Returns mutable reference to game state for save/load and pet system.
     * @return mutable reference to GameState
     */
    GameState& getMutableState();
    
    /**
     * Returns current game status (ONGOING, VICTORY, DEFEAT).
     * @return GameStatus enum value
     */
    GameStatus getStatus() const;
    
    /**
     * Returns sanity stage for event probability.
     * @return SanityStage enum value
     */
    SanityStage getSanityStage() const;
    
    /**
     * Returns difficulty configuration.
     * @return const reference to DifficultyConfig
     */
    const DifficultyConfig& getDifficultyConfig() const;
    
    /**
     * Returns days limit based on difficulty.
     * @return int daysLimit (or -1 for Story/unlimited)
     */
    int getDaysLimit() const;
    
    /**
     * Returns current number of days passed.
     * @return int currentDay
     */
    int getCurrentDay() const;
    
    // ========================================================================
    // EXTERNAL EVENT APPLICATION (Called by Role 3 & 4)
    // ========================================================================
    
    /**
     * Applies external event effects (loot, sanity/stamina from events).
     */
    void applyEventEffects(int foodGain, int waterGain, int goldGain, 
                           int sanityDelta, int staminaDelta, int durabilityDelta);
    
    /**
     * Applies pet interaction effects.
     */
    void applyPetInteraction(int moodGain, int sanityGain);
    
    /**
     * Applies market resupply purchases.
     */
    void applyMarketResupply(int foodPurchased, int waterPurchased, 
                             int durabilityRepaired, int goldSpent);
};

#endif // GAME_H