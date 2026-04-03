#include "Game.h"
#include <algorithm> // Required for std::max and std::min in C++11

// ============================================================================
// PRIVATE HELPER METHODS
// ============================================================================

void Game::applySanityDrain(int daysSpent) {
    int drain = difficultyConfig.sanityDrainPerDay * daysSpent;
    gameState.crew.sanity += drain;  // drain is already a negative number
    gameState.crew.sanity = clamp(gameState.crew.sanity, 0, 100);
}

// Note: applyFatiguePenalty() is bypassed because fatigue requires knowing 
// the specific days spent in the current action. Logic moved directly to sail().
void Game::applyFatiguePenalty() {} 

void Game::updateWaterResupplyCounter() {
    // Note: Replaced by inline logic in actions to accurately track days spent
}

bool Game::canPerformAction() const {
    return gameState.crew.stamina > 0;
}

void Game::checkWinLoseConditions() {
    // 1. Check failure conditions first (Using <= 0 is safer than == 0)
    if (gameState.crew.stamina <= 0 ||
        gameState.ship.durability <= 0 ||
        gameState.daysWithoutWaterResupply >= 2) {
        gameState.status = GameStatus::DEFEAT;
        return;
    }
    
    // 2. Check Deadline failure
    if (difficultyConfig.daysLimit != -1 && gameState.currentDay > difficultyConfig.daysLimit) {
        gameState.status = GameStatus::DEFEAT;
        return;
    }
    
    // 3. Check victory
    if (gameState.ship.distance >= 3000 && gameState.crew.stamina > 0) {
        gameState.status = GameStatus::VICTORY;
        return;
    }
    
    // Otherwise ongoing
    gameState.status = GameStatus::ONGOING;
}

int Game::clamp(int value, int min, int max) const {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// ============================================================================
// CONSTRUCTOR
// ============================================================================

Game::Game(Difficulty difficulty, bool hasPet)
    : gameState(difficulty, hasPet) {
    // Set up difficulty config
    switch (difficulty) {
        case Difficulty::STORY:
            difficultyConfig.daysLimit = -1;
            difficultyConfig.sanityDrainPerDay = -1;
            difficultyConfig.priceMultiplier = 0.8;
            break;
        case Difficulty::EASY:
            difficultyConfig.daysLimit = 92;
            difficultyConfig.sanityDrainPerDay = -1;
            difficultyConfig.priceMultiplier = 1.0;
            break;
        case Difficulty::MEDIUM:
            difficultyConfig.daysLimit = 61;
            difficultyConfig.sanityDrainPerDay = -2;
            difficultyConfig.priceMultiplier = 1.2;
            break;
        case Difficulty::HARD:
            difficultyConfig.daysLimit = 41;
            difficultyConfig.sanityDrainPerDay = -3;
            difficultyConfig.priceMultiplier = 1.5;
            break;
    }
}

// ============================================================================
// ACTION METHODS
// ============================================================================

ActionResult Game::sail(int daysToBeSailed) {
    ActionResult result;
    result.daysSpent = daysToBeSailed;
    result.distanceGained = 150 + (daysToBeSailed - 2) * 100;  // Deterministic: 150(2d), 250(3d), 350(4d)
    result.actionName = "Sail";

    // Base Effects
    result.staminaDelta = -1;
    result.foodDelta = -2;
    result.waterDelta = -2;
    result.durabilityDelta = -1;
    result.sanityDelta = 0;  
    result.goldDelta = 0;
    result.petFoodDelta = 0;
    result.petMoodDelta = 0;

    if (gameState.pet != nullptr) {
        result.petFoodDelta = -1;
    }

    // Apply Stat Updates
    gameState.crew.stamina = clamp(gameState.crew.stamina + result.staminaDelta, 0, 10);
    gameState.ship.durability = clamp(gameState.ship.durability + result.durabilityDelta, 0, 10);
    gameState.ship.distance += result.distanceGained;

    // Apply Resource Updates (Ensuring they never drop below 0)
    gameState.resources.food = std::max(0, gameState.resources.food + result.foodDelta);
    gameState.resources.freshWater = std::max(0, gameState.resources.freshWater + result.waterDelta);

    if (gameState.pet != nullptr) {
        gameState.pet->petFood = std::max(0, gameState.pet->petFood + result.petFoodDelta);
    }

    // --- FATIGUE CALCULATION FIX ---
    int oldConsecutiveDays = gameState.consecutiveSailingDays;
    gameState.consecutiveSailingDays += daysToBeSailed;
    gameState.currentDay += daysToBeSailed;

    int fatiguePenalty = 0;
    if (gameState.consecutiveSailingDays >= 5) {
        // Only penalize the days that exceed the 4-day limit during this specific action
        int penalizedDays = std::min(daysToBeSailed, gameState.consecutiveSailingDays - 4);
        if (penalizedDays > 0) {
            fatiguePenalty = -2 * penalizedDays;
            gameState.crew.sanity = clamp(gameState.crew.sanity + fatiguePenalty, 0, 100);
            result.sanityDelta += fatiguePenalty;
        }
    }

    // Apply passive sanity drain
    applySanityDrain(daysToBeSailed);
    result.sanityDelta += (difficultyConfig.sanityDrainPerDay * daysToBeSailed);

    // --- WATER RESUPPLY COUNTER FIX ---
    if (gameState.resources.freshWater <= 0) {
        gameState.daysWithoutWaterResupply += daysToBeSailed;
    } else {
        gameState.daysWithoutWaterResupply = 0;
    }

    // Handle Pet Mood logic
    if (gameState.pet != nullptr && gameState.pet->petFood == 0) {
        int moodDrop = -5 * daysToBeSailed;
        gameState.pet->petMood = clamp(gameState.pet->petMood + moodDrop, 0, 100);
        result.petMoodDelta += moodDrop;
    }

    checkWinLoseConditions();
    return result;
}

ActionResult Game::rest(int daysToRest) {
    ActionResult result;
    result.daysSpent = daysToRest;
    result.distanceGained = 0;
    result.actionName = "Rest";

    result.staminaDelta = 3;
    result.foodDelta = -1;
    result.waterDelta = 0;
    result.durabilityDelta = 0;
    result.sanityDelta = 5;  
    result.goldDelta = 0;
    result.petFoodDelta = 0;
    result.petMoodDelta = 0;

    gameState.crew.stamina = clamp(gameState.crew.stamina + result.staminaDelta, 0, 10);
    gameState.resources.food = std::max(0, gameState.resources.food + result.foodDelta);
    
    gameState.crew.sanity = clamp(gameState.crew.sanity + result.sanityDelta, 0, 100);
    
    gameState.consecutiveSailingDays = 0;  // Reset consecutive sailing
    gameState.currentDay += daysToRest;

    applySanityDrain(daysToRest);
    result.sanityDelta += (difficultyConfig.sanityDrainPerDay * daysToRest);

    if (gameState.resources.freshWater <= 0) {
        gameState.daysWithoutWaterResupply += daysToRest;
    } else {
        gameState.daysWithoutWaterResupply = 0;
    }

    if (gameState.pet != nullptr && gameState.pet->petFood == 0) {
        int moodDrop = -5 * daysToRest;
        gameState.pet->petMood = clamp(gameState.pet->petMood + moodDrop, 0, 100);
        result.petMoodDelta += moodDrop;
    }

    checkWinLoseConditions();
    return result;
}

ActionResult Game::explore(int daysToExplore) {
    ActionResult result;
    result.daysSpent = daysToExplore;
    result.distanceGained = 0;
    result.actionName = "Explore";

    result.staminaDelta = -1;
    result.foodDelta = -1;
    result.waterDelta = -1;
    result.durabilityDelta = 0;
    result.sanityDelta = 0; 
    result.goldDelta = 0;
    result.petFoodDelta = 0;
    result.petMoodDelta = 0;

    gameState.crew.stamina = clamp(gameState.crew.stamina + result.staminaDelta, 0, 10);
    gameState.resources.food = std::max(0, gameState.resources.food + result.foodDelta);
    gameState.resources.freshWater = std::max(0, gameState.resources.freshWater + result.waterDelta);

    gameState.currentDay += daysToExplore;

    applySanityDrain(daysToExplore);
    result.sanityDelta += (difficultyConfig.sanityDrainPerDay * daysToExplore);

    if (gameState.resources.freshWater <= 0) {
        gameState.daysWithoutWaterResupply += daysToExplore;
    } else {
        gameState.daysWithoutWaterResupply = 0;
    }

    if (gameState.pet != nullptr && gameState.pet->petFood == 0) {
        int moodDrop = -5 * daysToExplore;
        gameState.pet->petMood = clamp(gameState.pet->petMood + moodDrop, 0, 100);
        result.petMoodDelta += moodDrop;
    }

    checkWinLoseConditions();
    return result;
}

// ============================================================================
// GAME STATE QUERY METHODS
// ============================================================================

const GameState& Game::getGameState() const {
    return gameState;
}

GameStatus Game::getStatus() const {
    return gameState.status;
}

SanityStage Game::getSanityStage() const {
    int sanity = gameState.crew.sanity;
    if (sanity >= 80) return SanityStage::GOOD;
    if (sanity >= 40) return SanityStage::NORMAL;
    return SanityStage::CRITICAL;
}

const DifficultyConfig& Game::getDifficultyConfig() const {
    return difficultyConfig;
}

int Game::getDaysLimit() const {
    return difficultyConfig.daysLimit;
}

int Game::getCurrentDay() const {
    return gameState.currentDay;
}

// ============================================================================
// EXTERNAL EVENT APPLICATION
// ============================================================================

void Game::applyEventEffects(int foodGain, int waterGain, int goldGain,
                             int sanityDelta, int staminaDelta, int durabilityDelta) {
    gameState.resources.food = std::max(0, gameState.resources.food + foodGain);
    gameState.resources.freshWater = std::max(0, gameState.resources.freshWater + waterGain);
    gameState.resources.gold = std::max(0, gameState.resources.gold + goldGain);

    gameState.crew.sanity = clamp(gameState.crew.sanity + sanityDelta, 0, 100);
    gameState.crew.stamina = clamp(gameState.crew.stamina + staminaDelta, 0, 10);
    gameState.ship.durability = clamp(gameState.ship.durability + durabilityDelta, 0, 10);

    if (gameState.resources.freshWater > 0) {
        gameState.daysWithoutWaterResupply = 0;
    }

    checkWinLoseConditions();
}

void Game::applyPetInteraction(int moodGain, int sanityGain) {
    if (gameState.pet != nullptr) {
        gameState.pet->petMood = clamp(gameState.pet->petMood + moodGain, 0, 100);
    }
    gameState.crew.sanity = clamp(gameState.crew.sanity + sanityGain, 0, 100);
}

void Game::applyMarketResupply(int foodPurchased, int waterPurchased,
                               int durabilityRepaired, int goldSpent) {
    gameState.resources.food += foodPurchased;
    gameState.resources.freshWater += waterPurchased;
    gameState.resources.gold = std::max(0, gameState.resources.gold + goldSpent); // goldSpent is negative

    gameState.ship.durability = clamp(gameState.ship.durability + durabilityRepaired, 0, 10);

    if (gameState.resources.freshWater > 0) {
        gameState.daysWithoutWaterResupply = 0;
    }

    checkWinLoseConditions();
}