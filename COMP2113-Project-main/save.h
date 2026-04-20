#ifndef SAVE_H
#define SAVE_H

#include "game.h"
#include "event.h"

// ============================================================================
// SAVE SYSTEM NAMESPACE
// ============================================================================

// Structure to hold save file metadata (NEW)
struct SaveInfo
{
    bool exists;           // Whether save file exists
    bool hasPet;           // Whether player has a pet
    Difficulty difficulty; // Game difficulty setting
    int dayCount;          // Current day number
};

namespace SaveSystem
{

    /**
     * Saves the full game state to save.txt.
     * Format: versioned with header and line-by-line values.
     * @param game Current game state to save
     * @param sf Sanity/Fatigue tracking data
     * @return true on success
     */
    bool saveGame(const Game &game, const SanityFatigue &sf);

    /**
     * Checks if a valid save file exists.
     * Compatible with both old and new save formats.
     * @return true if save file exists and has valid header
     */
    bool hasSaveFile();

    /**
     * Peeks at the save file to read difficulty and hasPet
     * (needed to construct Game before loading full state).
     * @param diff Output parameter for difficulty setting
     * @param hasPet Output parameter for pet existence
     * @return true on success
     */
    bool peekSave(Difficulty &diff, bool &hasPet);

    /**
     * Gets detailed save file information without full load.
     * Useful for save selection menus or displaying save metadata.
     * @return SaveInfo structure containing save file metadata
     */
    SaveInfo getSaveInfo();

    /**
     * Loads full game state from save.txt into an already-constructed Game.
     * Supports both old and new save file versions.
     * @param game Game object to load state into
     * @param sf Sanity/Fatigue tracking data to load
     * @return true on success
     */
    bool loadGame(Game &game, SanityFatigue &sf);

    /**
     * Deletes the current save file.
     * Useful for starting a completely new game or resetting progress.
     * @return true if file was deleted or didn't exist
     */
    bool deleteSaveFile();

} // namespace SaveSystem

#endif // SAVE_H