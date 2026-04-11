/*
 * port.cpp
 * Role 4: Port / Market System
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey — The Yokohama Quest
 *
 * Handles port encounters and market transactions.
 * Ports appear every N km based on difficulty settings.
 */

#include "port.h"
#include "ui.h"
#include <iostream>
#include <limits>

using namespace std;

// ============================================================================
// PORT STATE
// ============================================================================

static int s_lastPortDistance = 0;

// ============================================================================
// HELPER: safe integer input
// ============================================================================

static int readPortInt() {
    int value;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Invalid input. Enter a number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// ============================================================================
// PORT NAMESPACE
// ============================================================================

namespace Port {

void resetPortTracker() {
    s_lastPortDistance = 0;
}

bool isAtPort(const Game& game) {
    const GameState& state = game.getGameState();
    GameDifficultySettings ds = getDifficultySettings(state.difficulty);

    int nextPort = s_lastPortDistance + ds.portEveryKm;
    return state.ship.distance >= nextPort;
}

void enterPort(Game& game, SanityFatigue& sf) {
    (void)sf;

    const GameState& state = game.getGameState();
    GameDifficultySettings ds = getDifficultySettings(state.difficulty);

    int nextPort = s_lastPortDistance + ds.portEveryKm;
    s_lastPortDistance = nextPort;

    int foodPrice  = static_cast<int>(5 * ds.priceMultiplier);
    int waterPrice = static_cast<int>(5 * ds.priceMultiplier);
    int repairCost = static_cast<int>(15 * ds.priceMultiplier);

    if (foodPrice < 1) foodPrice = 1;
    if (waterPrice < 1) waterPrice = 1;
    if (repairCost < 1) repairCost = 1;

    cout << "\n============================================" << endl;
    cout << "  You've arrived at a port! (" << nextPort << " km)" << endl;
    cout << "============================================" << endl;

    bool atPort = true;
    while (atPort) {
        cout << "\n--------------------------------------------" << endl;
        cout << "  Port Services" << endl;
        cout << "--------------------------------------------" << endl;
        cout << "  Gold: " << state.resources.gold << endl;
        cout << "--------------------------------------------" << endl;
        cout << "  [1] Buy Food     (" << foodPrice << " gold each)" << endl;
        cout << "  [2] Buy Water    (" << waterPrice << " gold each)" << endl;
        cout << "  [3] Repair Ship  (" << repairCost << " gold per point)" << endl;
        cout << "  [4] Leave Port" << endl;
        cout << "--------------------------------------------" << endl;
        cout << "  Your choice (1-4): ";

        int choice = readPortInt();
        while (choice < 1 || choice > 4) {
            cout << "  Invalid. Enter 1-4: ";
            choice = readPortInt();
        }

        switch (choice) {
            case 1: {
                int maxAfford = state.resources.gold / foodPrice;
                if (maxAfford <= 0) {
                    cout << "  You can't afford any food." << endl;
                    break;
                }
                cout << "  How many food to buy? (max " << maxAfford << "): ";
                int qty = readPortInt();
                if (qty < 0) qty = 0;
                if (qty > maxAfford) qty = maxAfford;
                if (qty > 0) {
                    game.applyMarketResupply(qty, 0, 0, -(qty * foodPrice));
                    cout << "  Bought " << qty << " food for "
                         << qty * foodPrice << " gold." << endl;
                }
                break;
            }
            case 2: {
                int maxAfford = state.resources.gold / waterPrice;
                if (maxAfford <= 0) {
                    cout << "  You can't afford any water." << endl;
                    break;
                }
                cout << "  How many water to buy? (max " << maxAfford << "): ";
                int qty = readPortInt();
                if (qty < 0) qty = 0;
                if (qty > maxAfford) qty = maxAfford;
                if (qty > 0) {
                    game.applyMarketResupply(0, qty, 0, -(qty * waterPrice));
                    cout << "  Bought " << qty << " water for "
                         << qty * waterPrice << " gold." << endl;
                }
                break;
            }
            case 3: {
                int maxRepair = 10 - state.ship.durability;
                if (maxRepair <= 0) {
                    cout << "  Ship is already at full durability." << endl;
                    break;
                }
                int maxAfford = state.resources.gold / repairCost;
                if (maxAfford <= 0) {
                    cout << "  You can't afford repairs." << endl;
                    break;
                }
                int maxQty = (maxRepair < maxAfford) ? maxRepair : maxAfford;
                cout << "  Repair how many points? (max " << maxQty << "): ";
                int qty = readPortInt();
                if (qty < 0) qty = 0;
                if (qty > maxQty) qty = maxQty;
                if (qty > 0) {
                    game.applyMarketResupply(0, 0, qty, -(qty * repairCost));
                    cout << "  Repaired " << qty << " durability for "
                         << qty * repairCost << " gold." << endl;
                }
                break;
            }
            case 4:
                cout << "  You leave the port and continue your voyage." << endl;
                atPort = false;
                break;
        }
    }
}

} // namespace Port
