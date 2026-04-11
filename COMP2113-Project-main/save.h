#ifndef SAVE_H
#define SAVE_H

#include "game.h"
#include "event.h"

// ============================================================================
// SAVE SYSTEM NAMESPACE
// ============================================================================

namespace SaveSystem {

    /**
     * Saves the full game state to save.txt.
     * @return true on success
     */
    bool saveGame(const Game& game, const SanityFatigue& sf);

    /**
     * Checks if a save file exists.
     */
    bool hasSaveFile();

    /**
     * Peeks at the save file to read difficulty and hasPet
     * (needed to construct Game before loading full state).
     * @return true on success
     */
    bool peekSave(Difficulty& diff, bool& hasPet);

    /**
     * Loads full game state from save.txt into an already-constructed Game.
     * @return true on success
     */
    bool loadGame(Game& game, SanityFatigue& sf);

} // namespace SaveSystem

#endif // SAVE_H
