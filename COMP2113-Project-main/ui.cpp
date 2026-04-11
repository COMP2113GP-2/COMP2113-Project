/*
 * ui.cpp
 * Role 5: UI / Input System
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey — The Yokohama Quest
 *
 * Handles all menus, status displays, input validation,
 * and screen management for the game.
 */
/*
 * ui.cpp
 * Role 5: UI / Input System
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey — The Yokohama Quest
 *
 * Handles all menus, status displays, input validation,
 * and screen management for the game.
 */

/*
 * ui.cpp
 * Role 5: UI / Input System
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey — The Yokohama Quest
 *
 * Handles all menus, status displays, input validation,
 * and screen management for the game.
 */
#undef max
#include "ui.h"
#include "pet.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <windows.h>

using namespace std;

// ============================================================================
// MODULE-LOCAL STATE
// ============================================================================

static int s_menuChoice = 0;

// ============================================================================
// ASCII ART RESOURCES (Adjusted width)
// ============================================================================

// Main menu navigation chart
static const char* MAIN_MENU_ART[] = {
    "    __    __    __    __    __    __    ",
    "   /  \\  /  \\  /  \\  /  \\  /  \\  /  \\ ",
    "  /  __\\/  __\\/  __\\/  __\\/  __\\/  __\\",
    " /  /  /  /  /  /  /  /  /  /  /  /  / ",
    "(  (  (  (  (  (  (  (  (  (  (  (  (  ",
    " \\  \\  \\  \\  \\  \\  \\  \\  \\  \\  \\  \\  \\ ",
    "  \\  \\  \\  \\  \\  \\  \\  \\  \\  \\  \\  \\  \\",
    "   \\__\\  \\__\\  \\__\\  \\__\\  \\__\\  \\__\\  ",
    " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
};

// Ship status art
static const char* SHIP_STATUS_ART[] = {
    "             |\\ | /|              ",
    "             | \\|/ |              ",
    "            /|     |\\             ",
    "           /_|     |_\\            ",
    "         oO_O       O_Oo          ",
    "         |_o_|     |_o_|          ",
    "           |_________|            ",
    "          /           \\           ",
    "         /             \\          ",
    "        /_______________\\         ",
    "   ~~~~~               ~~~~~      "
};

// Port sign
static const char* PORT_ART[] = {
    "       ____                     ",
    "     _|____|_                   ",
    "    /  o  o  \\                  ",
    "   |    ^     |  PORT           ",
    "   |   '-'    |  AHEAD          ",
    "    \\________/                  ",
    "      |    |                    ",
    "     /|    |\\                   ",
    "    / |    | \\                  ",
    "   ~~~      ~~~                 "
};

// Dog art
static const char* DOG_ART[] = {
    "      / \\__                     ",
    "     (    @\\___                 ",
    "     /         O                ",
    "    /   (_____/                 ",
    "   /_____/   U                  ",
    "                                ",
    "  DOGGO - Loyal Shipmate        "
};

// Cat art
static const char* CAT_ART[] = {
    "       /\\___/\\                  ",
    "      (  o o  )                 ",
    "       /  ^  \\                 ",
    "      /       \\                ",
    "      \\_______/                ",
    "       |     |                 ",
    "  KITTY - Nimble Companion     "
};

// Parrot art
static const char* PARROT_ART[] = {
    "        \\\\                     ",
    "         \\\\                    ",
    "         (o>                   ",
    "      \\\\_//)                   ",
    "       \\_/_)                   ",
    "        _|_                    ",
    "  POLLY - Colorful Lookout     "
};

// Victory sign
static const char* VICTORY_ART[] = {
    "    \\  |  /                   ",
    "     \\ | /                    ",
    "   .--\"\"\"--.                  ",
    "  /  _    _ \\                 ",
    " |  (o)  (o)  |  VICTORY!     ",
    "  \\    ^    /                 ",
    "   '--...--'                  ",
    "      |  |                    ",
    "     /    \\                   ",
    "    /      \\                  ",
    " ~~~~~~~~~~~~~~~              "
};

// Defeat sign
static const char* DEFEAT_ART[] = {
    "                              ",
    "     .-\"\"\"\"-._                ",
    "    /         \\               ",
    "   |  O     O  |  DEFEATED    ",
    "   |     v     |              ",
    "    \\  .---.  /              ",
    "     '-._____.-'               ",
    "       /     \\                ",
    "      /       \\               ",
    "     ~~~~~~~~~~~              "
};

// Compass
static const char* COMPASS_ART[] = {
    "         N                    ",
    "         |                    ",
    "         |                    ",
    "    W----+----E               ",
    "         |                    ",
    "         |                    ",
    "         S                    ",
    "                              ",
    "     YOKOHAMA  >>>            "
};

// ============================================================================
// TERMINAL HELPER FUNCTIONS
// ============================================================================

// Get terminal width
int getTerminalWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int width = 80; // Default width
    
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    
    // Ensure minimum width
    if (width < 60) width = 60;
    if (width > 120) width = 120; // Limit maximum width
    
    return width;
}

// Improved center printing function
void printCentered(const string& text, int width = -1) {
    if (width == -1) {
        width = getTerminalWidth();
    }
    
    int len = (int)text.size();
    
    // Handle empty lines
    if (len == 0) {
        cout << endl;
        return;
    }
    
    // If text is longer than width, output directly
    if (len >= width) {
        cout << text << endl;
        return;
    }
    
    // Calculate left margin
    int left = (width - len) / 2;
    
    // Output leading spaces
    for (int i = 0; i < left; i++) {
        cout << ' ';
    }
    
    cout << text << endl;
}

// Print divider line
void printDivider(const string& pattern = "=") {
    int width = getTerminalWidth();
    string line;
    
    // Create divider line
    while ((int)line.size() < width) {
        line += pattern;
    }
    
    // Trim to correct width
    cout << line.substr(0, width) << endl;
}

// Print wave line
void printWaveLine() {
    int width = getTerminalWidth();
    string wave;
    
    while ((int)wave.size() < width) {
        wave += "~";
    }
    
    cout << wave.substr(0, width) << endl;
}

// Print ASCII art
void printAsciiArt(const char* art[], int lines) {
    for (int i = 0; i < lines; i++) {
        printCentered(art[i]);
    }
}

// Print ASCII art based on pet type
void printPetArt(PetType type) {
    switch(type) {
        case PetType::CAT:
            printAsciiArt(CAT_ART, 7);
            break;
        case PetType::DOG:
            printAsciiArt(DOG_ART, 7);
            break;
        case PetType::PARROT:
            printAsciiArt(PARROT_ART, 7);
            break;
        default:
            // If no pet, don't print anything
            break;
    }
}

// Get pet type name
string getPetTypeName(PetType type) {
    switch(type) {
        case PetType::CAT: return "Cat";
        case PetType::DOG: return "Dog";
        case PetType::PARROT: return "Parrot";
        default: return "None";
    }
}

// Get pet type description
string getPetTypeDescription(PetType type) {
    switch(type) {
        case PetType::DOG: return "A loyal canine companion. Boosts crew morale.";
        case PetType::CAT: return "A nimble feline friend. Good at catching pests.";
        case PetType::PARROT: return "A colorful lookout. Can spot land from afar.";
        default: return "No companion selected.";
    }
}

// Print centered text with box
void printBoxedCentered(const string& text) {
    int width = getTerminalWidth();
    int textLen = (int)text.size();
    
    // Calculate appropriate box width
    int boxWidth = min(60, width - 4);
    if (textLen + 6 > boxWidth) {
        boxWidth = textLen + 6;
    }
    
    int leftPadding = (width - boxWidth) / 2;
    
    // Top border
    cout << string(leftPadding, ' ') << "╔" << string(boxWidth - 2, '═') << "╗" << endl;
    
    // Text line
    int leftTextPadding = (boxWidth - 2 - textLen) / 2;
    int rightTextPadding = boxWidth - 2 - textLen - leftTextPadding;
    
    cout << string(leftPadding, ' ') << "║" 
         << string(leftTextPadding, ' ') 
         << text 
         << string(rightTextPadding, ' ') 
         << "║" << endl;
    
    // Bottom border
    cout << string(leftPadding, ' ') << "╚" << string(boxWidth - 2, '═') << "╝" << endl;
}

// Print progress bar
void printProgressBar(int value, int maxValue, int width = 50) {
    int progress = (value * 100) / maxValue;
    if (progress > 100) progress = 100;
    if (progress < 0) progress = 0;
    
    int filled = (progress * width) / 100;
    
    string bar = "[";
    for (int i = 0; i < width; i++) {
        if (i < filled) bar += "█";
        else bar += "░";
    }
    bar += "] " + to_string(progress) + "%";
    
    printCentered(bar);
}

// ============================================================================
// CORE UI FUNCTIONS
// ============================================================================

namespace UI {

// ----------------------------------------------------------------------------
// clearScreen
// ----------------------------------------------------------------------------

void clearScreen() {
    system("cls");
}

// ----------------------------------------------------------------------------
// pressEnterToContinue
// ----------------------------------------------------------------------------

void pressEnterToContinue() {
    cout << endl;
    printWaveLine();
    printCentered("Press Enter to continue...");
     cin.ignore(INT_MAX, '\n'); 
}

// ----------------------------------------------------------------------------
// getInput
// ----------------------------------------------------------------------------

int getInput(int min, int max) {
    int value;
    while (true) {
        if (cin >> value && value >= min && value <= max) {
            cin.ignore(INT_MAX, '\n');
            return value;
        }
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        printCentered("Invalid input. Enter a number between " + 
                     to_string(min) + " and " + to_string(max) + ": ");
    }
}

// ----------------------------------------------------------------------------
// showMainMenu (ENHANCED WITH ASCII ART)
// ----------------------------------------------------------------------------

int showMainMenu() {
    clearScreen();
    
    // Top art
    printWaveLine();
    printAsciiArt(MAIN_MENU_ART, 9);
    printWaveLine();
    
    // Title
    printCentered("");
    printBoxedCentered("ABYSSAL ODYSSEY");
    printBoxedCentered("The Yokohama Quest");
    printCentered("");
    printCentered("A Text-Based Ocean Survival Adventure");
    printCentered("");
    
    // Ship art
    printAsciiArt(SHIP_STATUS_ART, 11);
    printCentered("");
    
    // Menu options
    printDivider("=");
    printCentered("[1] Set Sail  — Begin New Voyage");
    printCentered("[2] Load Ship — Continue Journey");
    printCentered("[3] Dock Ship — End Program");
    printDivider("=");
    printCentered("Your choice: ");
    
    return getInput(1, 3);
}

// ----------------------------------------------------------------------------
// showDifficultyMenu (ENHANCED)
// ----------------------------------------------------------------------------

int showDifficultyMenu() {
    clearScreen();
    
    printWaveLine();
    printCentered("     /\\                          ");
    printCentered("    /  \\    WEATHER  CHART      ");
    printCentered("   /    \\                        ");
    printCentered("  /------\\                       ");
    printWaveLine();
    
    printCentered("");
    printBoxedCentered("SELECT DIFFICULTY");
    printCentered("");
    
    // Compass art
    printAsciiArt(COMPASS_ART, 9);
    printCentered("");
    
    printDivider("=");
    printCentered("[1] Story   — Calm Seas, No Time Limit");
    printCentered("[2] Easy    — Gentle Winds, 92 Days");
    printCentered("[3] Medium  — Choppy Waters, 61 Days");
    printCentered("[4] Hard    — Stormy Ocean, 41 Days");
    printDivider("=");
    printCentered("Your choice: ");
    
    return getInput(1, 4);
}

// ----------------------------------------------------------------------------
// askPetChoice (Maintain compatibility with pet.cpp)
// ----------------------------------------------------------------------------

bool askPetChoice() {
    // This function maintains the original simple logic, does not modify the pet system
    printCentered("");
    printDivider("=");
    printCentered("Would you like a pet companion?");
    printDivider("=");
    printCentered("[1] Yes");
    printCentered("[2] No");
    printDivider("=");
    printCentered("Your choice: ");
    
    return getInput(1, 2) == 1;
}

// ----------------------------------------------------------------------------
// showDailyActionMenu (ENHANCED)
// ----------------------------------------------------------------------------

int showDailyActionMenu(bool hasPet) {
    printCentered("");
    printWaveLine();
    printCentered("DAILY LOG - Day " + to_string(g_pet.hasPet ? 1 : 0));
    printWaveLine();
    
    printAsciiArt(COMPASS_ART, 9);
    printCentered("");
    
    printDivider("=");
    printCentered("[1] SAIL    — Chart course to Yokohama");
    printCentered("[2] EXPLORE — Search islands & wrecks");
    printCentered("[3] REST    — Recover strength & morale");
    if (hasPet) {
        printCentered("[4] PET     — Care for your companion");
    }
    printDivider("=");
    printCentered("Your choice: ");

    int maxChoice = hasPet ? 4 : 3;
    return getInput(1, maxChoice);
}

// ----------------------------------------------------------------------------
// showPetMenu (ENHANCED WITH PET-SPECIFIC ART)
// ----------------------------------------------------------------------------

int showPetMenu() {
    clearScreen();
    
    // Check if there is a pet
    if (!g_pet.hasPet || !g_pet.alive) {
        printWaveLine();
        printCentered("No companion aboard");
        printWaveLine();
        pressEnterToContinue();
        return 4; // Return "Back"
    }
    
    printWaveLine();
    
    // Display pet art
    printPetArt(g_pet.type);
    printWaveLine();
    
    // Pet type description
    printCentered("");
    printCentered("Companion: " + g_pet.name + " (" + getPetTypeName(g_pet.type) + ")");
    printCentered(getPetTypeDescription(g_pet.type));
    printCentered("");
    
    printBoxedCentered("SHIP'S COMPANION MENU");
    printCentered("");
    
    printDivider("=");
    printCentered("[1] FEED  — Share rations with companion");
    printCentered("[2] PLAY  — Boost mood and bond");
    printCentered("[3] CHECK — View companion status");
    printCentered("[4] BACK  — Return to main actions");
    printDivider("=");
    printCentered("Your choice: ");
    
    return getInput(1, 4);
}

// ----------------------------------------------------------------------------
// showPortMenu (ENHANCED)
// ----------------------------------------------------------------------------

int showPortMenu() {
    clearScreen();
    
    printWaveLine();
    printAsciiArt(PORT_ART, 10);
    printWaveLine();
    
    printCentered("");
    printBoxedCentered("PORT OF CALL SERVICES");
    printCentered("");
    
    printDivider("=");
    printCentered("[1] TRADER   — Buy supplies & provisions");
    printCentered("[2] SHIPWRIGHT— Repair hull & equipment");
    printCentered("[3] RECORDS  — Save voyage progress");
    printCentered("[4] DEPART   — Set sail to continue");
    printDivider("=");
    printCentered("Your choice: ");
    
    return getInput(1, 4);
}

// ----------------------------------------------------------------------------
// displayStatus (ENHANCED WITH BETTER FORMATTING)
// ----------------------------------------------------------------------------

void displayStatus(const GameState& game, const SanityFatigue& sf) {
    clearScreen();
    
    int daysLimit = -1;
    switch (game.difficulty) {
        case Difficulty::STORY:  daysLimit = -1; break;
        case Difficulty::EASY:   daysLimit = 92; break;
        case Difficulty::MEDIUM: daysLimit = 61; break;
        case Difficulty::HARD:   daysLimit = 41; break;
    }
    
    // Top art
    printWaveLine();
    printAsciiArt(SHIP_STATUS_ART, 11);
    printWaveLine();
    
    // Title
    printCentered("");
    printBoxedCentered("VOYAGE STATUS REPORT");
    printCentered("");
    
    // Progress bar
    int progress = (game.ship.distance * 100) / 3000;
    if (progress > 100) progress = 100;
    if (progress < 0) progress = 0;
    printProgressBar(game.ship.distance, 3000);
    printCentered("");
    
    // Main status
    printDivider("-");
    printCentered("DAY: " + to_string(game.currentDay) + 
                 (daysLimit > 0 ? " / " + to_string(daysLimit) : " (No Limit)"));
    printCentered("DISTANCE: " + to_string(game.ship.distance) + " / 3000 km");
    printDivider("-");
    
    // Resources
    printCentered("SUPPLIES: " + to_string(game.resources.food));
    printCentered("WATER:    " + to_string(game.resources.freshWater));
    printCentered("GOLD:     " + to_string(game.resources.gold));
    printDivider("-");
    
    // Crew status
    printBoxedCentered("CREW CONDITION");
    printCentered("");
    
    // Stamina bar
    string staminaBar = "STAMINA:   [";
    for (int i = 0; i < 10; i++) {
        if (i < game.crew.stamina) staminaBar += "█";
        else staminaBar += "░";
    }
    staminaBar += "] " + to_string(game.crew.stamina) + "/10";
    printCentered(staminaBar);
    
    // Sanity bar
    string sanityBar = "SANITY:    [";
    int sanityBars = game.crew.sanity / 10;
    for (int i = 0; i < 10; i++) {
        if (i < sanityBars) {
            if (game.crew.sanity >= 80) sanityBar += "▓";
            else if (game.crew.sanity >= 40) sanityBar += "▒";
            else sanityBar += "░";
        }
        else sanityBar += "░";
    }
    sanityBar += "] " + to_string(game.crew.sanity) + "/100";
    if (game.crew.sanity < 40) sanityBar += "  [CRITICAL]";
    else if (game.crew.sanity >= 80) sanityBar += "  [GOOD]";
    printCentered(sanityBar);
    
    // Hull durability
    string durabilityBar = "HULL:      [";
    for (int i = 0; i < 10; i++) {
        if (i < game.ship.durability) durabilityBar += "█";
        else durabilityBar += "░";
    }
    durabilityBar += "] " + to_string(game.ship.durability) + "/10";
    printCentered(durabilityBar);
    
    // Fatigue status
    if (sf.consecutiveSailingDays >= 5) {
        printCentered("FATIGUE:    ██████████  FATIGUED (" + 
                     to_string(sf.consecutiveSailingDays) + " days)");
    } else if (sf.consecutiveSailingDays >= 3) {
        printCentered("FATIGUE:    ████░░░░░░  WARNING (" + 
                     to_string(sf.consecutiveSailingDays) + " days)");
    }
    
    // Pet status
    if (g_pet.hasPet && g_pet.alive && game.pet != nullptr) {
        printDivider("-");
        printCentered("SHIP'S COMPANION: " + g_pet.name + 
                     " (" + getPetTypeName(g_pet.type) + ")");
        
        // Display corresponding pet art
        printPetArt(g_pet.type);
        
        string petFoodBar = "FOOD:  [";
        int petFoodBars = game.pet->petFood / 20;
        for (int i = 0; i < 5; i++) {
            if (i < petFoodBars) petFoodBar += "▓";
            else petFoodBar += "░";
        }
        petFoodBar += "] " + to_string(game.pet->petFood) + "/100";
        
        string petMoodBar = "MOOD:  [";
        int petMoodBars = game.pet->petMood / 20;
        for (int i = 0; i < 5; i++) {
            if (i < petMoodBars) {
                if (game.pet->petMood >= 60) petMoodBar += "♥";
                else if (game.pet->petMood >= 30) petMoodBar += "♡";
                else petMoodBar += "♢";
            }
            else petMoodBar += "░";
        }
        petMoodBar += "] " + to_string(game.pet->petMood) + "/100";
        
        printCentered(petFoodBar + "  " + petMoodBar);
    }
    
    printWaveLine();
    printCentered("");
}

// ----------------------------------------------------------------------------
// showWinScreen (ENHANCED WITH PET-SPECIFIC MESSAGES)
// ----------------------------------------------------------------------------

void showWinScreen(int daysUsed, bool bff) {
    clearScreen();
    
    printWaveLine();
    printAsciiArt(VICTORY_ART, 11);
    printWaveLine();
    
    printCentered("");
    printBoxedCentered("*** VICTORY! ***");
    printCentered("");
    
    printCentered("YOKOHAMA HARBOR - SAFELY ARRIVED");
    printCentered("");
    
    // Ship arrival art
    printCentered("        |\\   |   /|        ");
    printCentered("        | \\  |  / |        ");
    printCentered("       /|  \\ | /  |\\       ");
    printCentered("      /_|   \\|/   |_\\      ");
    printCentered("   oO_O          O_Oo       ");
    printCentered("   |_o_|  DOCKED |_o_|     ");
    printCentered("     |____________|        ");
    printCentered("    /              \\       ");
    printCentered("   /  THE  VOYAGE   \\      ");
    printCentered("  /     IS  COMPLETE \\     ");
    printCentered(" ~~~~~~~~~~~~~~~~~~~~~~~~~ ");
    printCentered("");
    
    printDivider("=");
    printCentered("DAYS AT SEA: " + to_string(daysUsed));
    printCentered("DISTANCE SAILED: 3000 km");
    
    // Display pet-related victory message
    if (g_pet.hasPet && g_pet.alive) {
        printDivider("-");
        printCentered("With your trusty " + getPetTypeName(g_pet.type) + 
                     " " + g_pet.name + " by your side!");
        printPetArt(g_pet.type);
    }
    
    if (bff) {
        printDivider("-");
        printCentered("★ ACHIEVEMENT UNLOCKED: BEST FRIEND FOREVER ★");
        if (g_pet.hasPet) {
            printCentered("Your faithful " + getPetTypeName(g_pet.type) + " " + g_pet.name);
            printCentered("stayed by your side through every storm and starlit night.");
        } else {
            printCentered("Your crew bonded as a family on this incredible journey.");
        }
        printCentered("");
    }
    
    printDivider("=");
    printCentered("The Abyssal Odyssey has reached its end.");
    printCentered("Fair winds, Captain. Until next voyage.");
    printWaveLine();
}

// ----------------------------------------------------------------------------
// showLoseScreen (ENHANCED)
// ----------------------------------------------------------------------------

void showLoseScreen(const string& reason) {
    clearScreen();
    
    printWaveLine();
    printAsciiArt(DEFEAT_ART, 10);
    printWaveLine();
    
    printCentered("");
    printBoxedCentered("*** VOYAGE LOST ***");
    printCentered("");
    
    // Shipwreck art
    printCentered("         .-\"\"\"\"-._                ");
    printCentered("        /         \\               ");
    printCentered("       |           |              ");
    printCentered("       |  x     x  |              ");
    printCentered("        \\  .---.  /              ");
    printCentered("         '-._____.-'               ");
    printCentered("             |  |                  ");
    printCentered("           \\/    \\/             ");
    printCentered("         ~~~~~~~~~~~~~           ");
    printCentered("");
    
    printDivider("=");
    printCentered("The ocean claims another voyage...");
    printCentered("");
    printCentered("CAUSE: " + reason);
    printCentered("");
    
    // Display pet-related failure message
    if (g_pet.hasPet && g_pet.alive) {
        printDivider("-");
        printCentered("Your " + getPetTypeName(g_pet.type) + " " + 
                     g_pet.name + " will be missed...");
        printPetArt(g_pet.type);
    }
    
    printDivider("=");
    printCentered("May your next journey find calmer seas.");
    printCentered("The Abyssal Odyssey continues...");
    printWaveLine();
}

// ============================================================================
// WRAPPER FUNCTIONS
// ============================================================================

void showWelcomeScreen() {
    s_menuChoice = showMainMenu();
    if (s_menuChoice == 3) {
        cout << endl;
        printWaveLine();
        printCentered("Ship safely docked. Until next voyage, Captain.");
        printWaveLine();
        cout << endl;
        exit(0);
    }
}

int askLoadGame() {
    return (s_menuChoice == 2) ? 1 : 2;
}

int chooseDifficulty() {
    return showDifficultyMenu();
}

void showGameStatus(const GameState& game, const SanityFatigue& sf) {
    displayStatus(game, sf);
}

int playerActionMenu() {
    return showDailyActionMenu(g_pet.hasPet);
}

int playerPetMenu() {
    return showPetMenu();
}

int askSaveGame() {
    clearScreen();
    printWaveLine();
    printCentered("      ___                     ");
    printCentered("     /   \\___   RECORD LOG   ");
    printCentered("    /        \\               ");
    printCentered("   /  SAVE    \\              ");
    printCentered("  /  PROGRESS  \\             ");
    printCentered("  \\___________/              ");
    printWaveLine();
    
    printCentered("");
    printBoxedCentered("RECORD VOYAGE PROGRESS?");
    printCentered("");
    
    printDivider("=");
    printCentered("[1] Yes — Save journey to ship's log");
    printCentered("[2] No  — Continue without saving");
    printDivider("=");
    printCentered("Your choice: ");
    
    return getInput(1, 2);
}

void showGameResult(const GameState& game) {
    if (game.status == GameStatus::VICTORY) {
        showWinScreen(game.currentDay, g_pet.bffUnlocked);
    } else {
        string reason;
        if (game.crew.stamina <= 0)
            reason = "Crew succumbed to exhaustion";
        else if (game.ship.durability <= 0)
            reason = "Ship broken by the relentless sea";
        else if (game.daysWithoutWaterResupply >= 2)
            reason = "Dehydration took the crew";
        else if (game.currentDay >= 100)  // Assuming 100 is max
            reason = "Time ran out before reaching port";
        else
            reason = "The voyage met an untimely end";
        showLoseScreen(reason);
    }
}

} // namespace UI
