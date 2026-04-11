#ifndef PORT_H
#define PORT_H

#include "game.h"
#include "event.h"

// ============================================================================
// PORT NAMESPACE
// ============================================================================

namespace Port {

    /**
     * Checks if the ship has reached a new port based on distance and difficulty.
     * @param game Game object to read distance and difficulty
     * @return true if a port is available
     */
    bool isAtPort(const Game& game);

    /**
     * Runs the full port interaction: buy supplies, repair, leave.
     * @param game Game object (modified via applyMarketResupply)
     * @param sf SanityFatigue reference (unused, kept for API compat)
     */
    void enterPort(Game& game, SanityFatigue& sf);

    /**
     * Resets port tracking (call when starting a new game or loading).
     */
    void resetPortTracker();

} // namespace Port

#endif // PORT_H
