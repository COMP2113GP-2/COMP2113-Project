/*
 * ui_display.cpp
 * Role 3: ASCII Ship Display & Animation System
 * Course: COMP2113 / ENGG1340
 * Project: Abyssal Odyssey
 */

#include "ui_display.h"
#include "game.h"
#include "event.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <unistd.h>

using namespace std;

static const string RST = "\033[0m";
static const string BOLD = "\033[1m";
static const string DIM = "\033[2m";
static const string FG_RED = "\033[31m";
static const string FG_GREEN = "\033[32m";
static const string FG_GOLD = "\033[33m";
static const string FG_BLUE = "\033[34m";
static const string FG_MAG = "\033[35m";
static const string FG_CYAN = "\033[36m";
static const string FG_WHITE = "\033[37m";
static const string FG_ORANGE = "\033[38;5;214m";
static const string FG_LTBLUE = "\033[38;5;39m";
static const string FG_LTCYAN = "\033[38;5;51m";
static const string FG_BROWN = "\033[38;5;130m";
static const string FG_GRAY = "\033[38;5;245m";

static void sleepMs(int ms) { usleep(ms * 1000); }

/*
 * Progress bar using block character via hex escape.
 * Input:  val, max, width, fillColor
 * Output: bar string
 */
static string makeBar(int val, int max, int width, const string& fillColor) {
    if (max <= 0) max = 1;
    int filled = (int)((double)val / max * width);
    if (filled > width) filled = width;
    if (filled < 0)     filled = 0;
    string bar = "[" + fillColor;
    for (int i = 0; i < width; i++) {
        if (i < filled) bar += "\xe2\x96\x88";
        else            bar += DIM + string(".") + fillColor;
    }
    bar += RST + "]";
    return bar;
}

/*
 * Count visible (non-ANSI, non-UTF8-continuation) characters.
 */
static int visibleLen(const string& s) {
    int len = 0; bool inEsc = false;
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = (unsigned char)s[i];
        if (c == 0x1B) { inEsc = true; continue; }
        if (inEsc) { if (s[i] == 'm') inEsc = false; continue; }
        if (c >= 0x80 && c < 0xC0) continue; // UTF-8 continuation
        len++;
    }
    return len;
}

/*
 * Pad string to visible width.
 */
static string padTo(const string& s, int width) {
    int vlen = visibleLen(s);
    if (vlen >= width) return s;
    return s + string(width - vlen, ' ');
}

// ============================================================================
// SHIP FRAME (top-down view with clean border)
// ============================================================================

/*
 * Builds top-view ship ASCII art.
 * Input:  wavePhase 0/1, stormy bool
 * Output: vector of 15 colored strings
 */
static vector<string> buildShipFrame(int wavePhase, bool stormy) {
    string wa = stormy ? "~^~" : (wavePhase == 0 ? "~~~" : "^^^");
    string wb = stormy ? "^~^" : (wavePhase == 0 ? "^^^" : "~~~");

    string wc = stormy ? FG_CYAN : FG_LTBLUE;
    string sc = FG_BROWN;
    string mc = FG_GOLD;
    string slc = FG_WHITE;

    const int INNER_W = 21;
    const int TOTAL_W = 36;

    vector<string> f;

    auto fill = [&](int n) {
        string s;
        for (int i = 0; i < n; ++i)
            s += (i % 2 ? "^" : "~");
        return s;
    };

    auto padMid = [&](const string& mid) {
        int pad = INNER_W - (int)mid.size();
        int l = pad / 2;
        int r = pad - l;
        return fill(l) + mid + fill(r);
    };

    auto build = [&](string mid, string l, string r, bool hasShip) {

        string left = l + l;
        string right = r + r;
        string midP = padMid(mid);

        string line;

        line += wc + left + " " + RST;

        for (char c : midP) {
            if (c == '~' || c == '^') {
                line += wc;
                line += c;
                line += RST;
            } else if (hasShip) {
                line += sc;
                line += c;
                line += RST;
            } else {
                line += c;
            }
        }

        line += wc + " " + right + RST;

        int visible = left.size() + 1 + midP.size() + 1 + right.size();
        if (visible < TOTAL_W)
            line += string(TOTAL_W - visible, ' ');

        return line;
    };

    f.push_back(build("", wa, wb, false));
    f.push_back(build("", wb, wa, false));

    f.push_back(build(" _|_ ", wa, wb, true));
    f.push_back(build(",' | `,", wb, wa, true));
    f.push_back(build("/   |   \\", wa, wb, true));
    f.push_back(build("|   _|_   |", wb, wa, true));
    f.push_back(build("| ,',|,`, |", wa, wb, true));
    f.push_back(build("|//_|||_\\\\|", wb, wa, true));
    f.push_back(build(" ||',-\"-\",`||", wa, wb, true));
    f.push_back(build("||/     \\||", wb, wa, true));
    f.push_back(build(" \\|     |/", wa, wb, true));
    f.push_back(build(" `,___,'", wb, wa, true));

    f.push_back(build("  ~~~~~~~", wa, wb, false));

    string waveMid = (wavePhase == 0 ? "~-~-~-~-~" : "--~-~-~-~");
    f.push_back(build(waveMid, wb, wa, false));

    f.push_back(build("", wa, wb, false));

    return f;
}





// ============================================================================
// TITLE BANNER
// ============================================================================
/*
 * Prints animated title banner using only ASCII characters.
 * Input:  none
 * Output: title to stdout
 */
void Display::showTitleBanner() {
    cout << "\033[2J\033[H";
    cout << FG_GOLD + BOLD;
    cout << " _____                                                             _____ \n";
    cout << "( ___ )-----------------------------------------------------------( ___ )\n";
    cout << " |   |                                                             |   |\n";
    cout << " |   |" << FG_CYAN  << "   █████╗ ██████╗ ██╗   ██╗███████╗███████╗ █████╗ ██╗       " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_CYAN  << "  ██╔══██╗██╔══██╗╚██╗ ██╔╝██╔════╝██╔════╝██╔══██╗██║       " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_CYAN  << "  ███████║██████╔╝ ╚████╔╝ ███████╗███████╗███████║██║       " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_CYAN  << "  ██╔══██║██╔══██╗  ╚██╔╝  ╚════██║╚════██║██╔══██║██║       " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_CYAN  << "  ██║  ██║██████╔╝   ██║   ███████║███████║██║  ██║███████╗  " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_CYAN  << "  ╚═╝  ╚═╝╚═════╝    ╚═╝   ╚══════╝╚══════╝╚═╝  ╚═╝╚══════╝  " << FG_GOLD << "|   |\n";
    cout << " |   |                                                             |   |\n";
    cout << " |   |" << FG_WHITE << "  ██████╗ ██████╗ ██╗   ██╗███████╗███████╗███████╗██╗   ██╗ " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_WHITE << " ██╔═══██╗██╔══██╗╚██╗ ██╔╝██╔════╝██╔════╝██╔════╝╚██╗ ██╔╝ " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_WHITE << " ██║   ██║██║  ██║ ╚████╔╝ ███████╗███████╗█████╗   ╚████╔╝  " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_WHITE << " ██║   ██║██║  ██║  ╚██╔╝  ╚════██║╚════██║██╔══╝    ╚██╔╝   " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_WHITE << " ╚██████╔╝██████╔╝   ██║   ███████║███████║███████╗   ██║    " << FG_GOLD << "|   |\n";
    cout << " |   |" << FG_WHITE << "  ╚═════╝ ╚═════╝    ╚═╝   ╚══════╝╚══════╝╚══════╝   ╚═╝    " << FG_GOLD << "|   |\n";
    cout << " |   |                                                             |   |\n";
    cout << " |___|        " << FG_LTBLUE << "    Hong Kong " << FG_GOLD << ">>>>>>>>>>>>>>>>>> " << FG_LTBLUE << " Yokohama" << FG_GOLD << "           |___|\n";
    cout << "(_____)" << FG_GRAY << "-----------------------------------------------------------" << FG_GOLD << "(_____)\n";
    cout << RST << "\n";

    for (int f = 0; f < 4; f++) {
        string wave = (f % 2 == 0)
            ? "~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^"
            : "^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^~^";
        cout << "\r  " << FG_LTBLUE << wave << RST << "  " << flush;
        sleepMs(220);
    }
    cout << "\n\n";
}

// ============================================================================
// GAME HUD
// ============================================================================

/*
 * Draws full game HUD.
 * Input:  game, sf, lastOutcome
 * Output: HUD to stdout
 */
void Display::showGameHUD(const GameState& game, const SanityFatigue& sf,
                          const string& lastOutcome) {
    cout << "\033[2J\033[H";

    // Last turn outcome shown at the very top — always visible
    if (!lastOutcome.empty()) {
        cout << FG_GOLD + BOLD + " +----------------------------------------------------------------------+\n" + RST;
        cout << FG_GOLD + BOLD + " | LAST TURN:" + RST + " "
             + padTo(lastOutcome, 58)
             + FG_GOLD + BOLD + " |\n" + RST;
        cout << FG_GOLD + BOLD + " +----------------------------------------------------------------------+\n" + RST;
        cout << "\n";
    }

    string diffColor, diffName;
    int daysLimit = -1;
    switch (game.difficulty) {
        case Difficulty::STORY:  diffColor=FG_CYAN;   diffName="STORY";  daysLimit=-1; break;
        case Difficulty::EASY:   diffColor=FG_GREEN;  diffName="EASY";   daysLimit=92; break;
        case Difficulty::MEDIUM: diffColor=FG_ORANGE; diffName="MEDIUM"; daysLimit=61; break;
        case Difficulty::HARD:   diffColor=FG_RED;    diffName="HARD";   daysLimit=41; break;
        default:                 diffColor=FG_WHITE;  diffName="?";      break;
    }

    string weather;
    if      (game.crew.sanity >= 80) weather = FG_GOLD + string("[ SUNNY  ]");
    else if (game.crew.sanity >= 50) weather = FG_CYAN + string("[ CLOUDY ]");
    else if (game.crew.sanity >= 30) weather = FG_BLUE + string("[ STORMY ]");
    else                             weather = FG_RED  + string("[  DIRE  ]");

    cout << FG_GOLD + BOLD + " +======================================================================+\n" + RST;

    char dayBuf[16], distBuf[16], limitBuf[16];
    snprintf(dayBuf,   sizeof(dayBuf),   "%4d", game.currentDay);
    snprintf(distBuf,  sizeof(distBuf),  "%4d", game.ship.distance);
    snprintf(limitBuf, sizeof(limitBuf), "%2d",  daysLimit);

    string dayStr = FG_WHITE + BOLD + string(dayBuf) + RST;
    string distStr = FG_LTBLUE + BOLD +string(distBuf) + "/3000km" + RST;
    string limitStr = (daysLimit > 0)
        ? FG_GRAY + string("/") + limitBuf + RST
        : string("      ");
    string diffStr = diffColor + BOLD + padTo(diffName, 6) + RST;

    cout << FG_GOLD + BOLD + " |  OCEAN VOYAGE  |  " + RST
         << FG_WHITE + "Day: " << dayStr << limitStr
         << FG_GRAY + "|  " << distStr
         << FG_GRAY + "  |  " << diffStr
         << FG_GRAY + "  |  " << weather << RST
         << FG_GOLD + BOLD + "  |\n" + RST;
    cout << FG_GOLD + BOLD + " +======================================================================+\n" + RST;
    cout << "\n";

    bool stormy = (game.crew.sanity < 30 || game.ship.durability <= 3);
    vector<string> ship = buildShipFrame(game.currentDay % 2, stormy);

    auto statBar = [&](const string& label, int val, int maxVal,
                       const string& color) -> string {
        string barColor = color;
        if (maxVal == 10  && val <= 3)  barColor = FG_RED;
        if (maxVal == 100 && val <= 39) barColor = FG_RED;
        string b = " " + padTo(label, 11) + " "
                 + barColor + makeBar(val, maxVal, 14, barColor) + RST
                 + " " + barColor + to_string(val) + RST;
        if (maxVal <= 10)
            b += FG_GRAY + "/" + to_string(maxVal) + RST;
        return b;
    };

    vector<string> left;
    left.push_back(FG_GOLD + BOLD + " CREW & SHIP" + RST);
    left.push_back(FG_GOLD + " ------------" + RST);
    left.push_back(statBar("Stamina",    game.crew.stamina,         10,  FG_GREEN));
    left.push_back(statBar("Sanity",     game.crew.sanity,          100, FG_MAG));
    left.push_back(statBar("Food",       game.resources.food,       40,  FG_ORANGE));
    left.push_back(statBar("Water",      game.resources.freshWater, 40,  FG_LTBLUE));
    left.push_back(statBar("Durability", game.ship.durability,      10,  FG_GRAY));
    left.push_back(statBar("Gold",       game.resources.gold,       500, FG_GOLD));
    left.push_back("");
    left.push_back(FG_GOLD + BOLD + " FATIGUE" + RST);
    left.push_back(FG_GOLD + " --------" + RST);

    string dots = " ";
    for (int i = 0; i < 7; i++) {
        if (i < sf.consecutiveSailingDays)
            dots += (i >= 4) ? FG_RED + string("[!]") + RST
                              : FG_ORANGE + string("[#]") + RST;
        else
            dots += FG_GRAY + DIM + string("[ ]") + RST;
    }
    left.push_back(dots);
    left.push_back(sf.consecutiveSailingDays >= 5
        ? FG_RED + BOLD + " !! -2 Sanity/day  +20% bad events" + RST
        : FG_GRAY + DIM + " Rest after 4 days to avoid fatigue" + RST);
    left.push_back("");
    left.push_back(FG_GOLD + BOLD + " SANITY STAGE" + RST);
    left.push_back(FG_GOLD + " -------------" + RST);

    if (game.crew.sanity >= 80) {
        left.push_back(FG_CYAN  + BOLD + " >> HIGH (80-100)" + RST);
        left.push_back(FG_CYAN  + "    2 actions next turn!" + RST);
    } else if (game.crew.sanity >= 40) {
        left.push_back(FG_GREEN + " >> NORMAL (40-79)" + RST);
        left.push_back(FG_GRAY  + DIM + "    Standard gameplay." + RST);
    } else {
        left.push_back(FG_RED   + BOLD + " >> CRITICAL (0-39)" + RST);
        left.push_back(FG_RED   + "    +5% bad event chance!" + RST);
    }

    if (game.daysWithoutWaterResupply > 0) {
        left.push_back("");
        left.push_back(FG_RED + BOLD + " !! NO WATER: "
                       + to_string(game.daysWithoutWaterResupply) + "/2 days !!" + RST);
    }

    if (game.pet != nullptr) {
        left.push_back("");
        left.push_back(FG_MAG + BOLD + " PET" + RST);
        left.push_back(FG_MAG + " ----" + RST);
        left.push_back(statBar("Pet Food", game.pet->petFood, 100, FG_ORANGE));
        left.push_back(statBar("Pet Mood", game.pet->petMood, 100, FG_MAG));
    }

    int rows = (int)(left.size() > ship.size() ? left.size() : ship.size());
    cout << "\n";
    for (int i = 0; i < rows; i++) {
        string leftLine = (i < (int)left.size()) ? left[i] : "";
        cout << padTo(leftLine, 40);
        if (i < (int)ship.size()) cout << "  " << ship[i];
        cout << "\n";
    }

    // Progress track
    const int INNER = 68;
    cout << "\n";
    cout << FG_GOLD + BOLD + " +" + string(INNER, '-') + "+\n" + RST;
    cout << FG_GOLD + BOLD + " | VOYAGE PROGRESS" + string(INNER - 16, ' ') + "|\n" + RST;
    cout << FG_GOLD + BOLD + " +" + string(INNER, '-') + "+\n" + RST;

    const int TW = INNER - 4;
    int shipPos = (int)((double)game.ship.distance / 3000.0 * TW);
    if (shipPos > TW) shipPos = TW;

    string track = " |  ";
    for (int i = 0; i < TW; i++) {
        bool isPort = (i == TW/6 || i == TW/3 || i == TW/2
                       || i == 2*TW/3 || i == 5*TW/6);
        if (i == shipPos)     track += FG_GOLD + BOLD + ">" + RST;
        else if (isPort)      track += FG_CYAN + "|" + RST;
        else if (i < shipPos) track += FG_LTBLUE + "=" + RST;
        else                  track += FG_GRAY + DIM + "." + RST;
    }
    track += "  |";
    cout << FG_GOLD + BOLD << track << RST << "\n";

    int gap = TW / 6 - 3;
    string portRow = " |  HKG";
    portRow += string(gap,' ') + FG_CYAN + "+P1" + RST;
    portRow += string(gap,' ') + FG_CYAN + "+P2" + RST;
    portRow += string(gap,' ') + FG_CYAN + "+P3" + RST;
    portRow += string(gap,' ') + FG_CYAN + "+P4" + RST;
    portRow += string(gap,' ') + FG_CYAN + "+P5" + RST;
    portRow += string(3,' ') + FG_GRAY + "     YKH  |" + RST;
    cout << portRow << "\n";

    char distLabel[32];
    snprintf(distLabel, sizeof(distLabel), "%4d km (%3d%%)",
             game.ship.distance,
             (int)((double)game.ship.distance / 3000.0 * 100));
    int midPad = (TW - (int)strlen(distLabel)) / 2;
    if (midPad < 4) midPad = 4;
    string distRow = " | 0 km" + string(midPad - 4, ' ')
                   + FG_LTBLUE + string(distLabel) + RST
                   + string(midPad - 4, ' ') + "3000 km |";
    cout << distRow << "\n";
    cout << FG_GOLD + BOLD + " +" + string(INNER, '-') + "+\n" + RST;
    cout << "\n";
}

// ============================================================================
// SAIL ANIMATION
// ============================================================================

void Display::sailAnimation(int distanceGained, int days) {
    cout << "\033[2J\033[H";
    static const char* SHIP[4] = {
        "      |      ",
        "     /|\\     ",
        "    / | \\    ",
        "###/##|##\\###"
    };
    for (int f = 0; f < 16; f++) {
        cout << "\033[H";
        string stars = FG_GRAY + DIM + "  ";
        int off = f % 12;
        for (int s = 0; s < 6; s++)
            stars += string((s * 11 + off) % 56, ' ') + ". ";
        cout << stars << RST << "\n";
        cout << FG_GRAY + DIM
             + "  - - - - - - - - - - - - - - - - - - - - - - - -\n" + RST;
        cout << "\n" << FG_GOLD + BOLD
             << "  === SAILING === +" << distanceGained << " km === "
             << days << " day(s) ===\n" << RST << "\n";
        int shipX = f * 3; if (shipX > 44) shipX = 44;
        string sp(shipX, ' ');
        cout << sp << FG_WHITE << SHIP[0] << RST << "\n";
        cout << sp << FG_WHITE << SHIP[1] << RST << "\n";
        cout << sp << FG_WHITE << SHIP[2] << RST << "\n";
        cout << sp << FG_BROWN << SHIP[3] << RST << "\n\n";
        string wv1, wv2;
        int wOff = f % 4;
        for (int w = 0; w < 28; w++) {
            bool alt = ((w + wOff) % 2 == 0);
            wv1 += alt ? FG_LTBLUE+"~^"+RST : FG_CYAN+"^~"+RST;
            wv2 += alt ? FG_BLUE+"^^"+RST   : FG_LTBLUE+"~~"+RST;
        }
        cout << "  " << wv1 << "\n" << "  " << wv2 << "\n" << "  " << wv1 << "\n\n";
        int lo = 38 - f*2; if (lo < 0) lo = 0;
        int lo2 = lo + 5;  if (lo2 > 50) lo2 = 50;
        cout << FG_LTCYAN+DIM+string(lo,' ')+"- - - - - - - - - - - - - -\n"+RST;
        cout << FG_CYAN+DIM+string(lo2,' ')+"- - - - - - - - - -\n"+RST;
        cout << flush;
        sleepMs(75);
    }
    sleepMs(200);
}

// ============================================================================
// EXPLORE ANIMATION
// ============================================================================

void Display::exploreAnimation() {
    cout << "\033[2J\033[H";
    static const char* SCOPE[5] = {
        "      _____     ",
        "     /     \\    ",
        "    | o   o |   ",
        "     \\_____/    ",
        "        |       "
    };
    for (int f = 0; f < 14; f++) {
        cout << "\033[H";
        cout << "\n\n" << FG_GOLD + BOLD
             << "  =====  EXPLORING  =====  Searching the horizon...\n"
             << RST << "\n";
        int bob = (f % 4 < 2) ? 0 : 1;
        for (int blank = 0; blank < bob; blank++) cout << "\n";
        for (int r = 0; r < 5; r++)
            cout << "   " << FG_GOLD << SCOPE[r] << RST << "\n";
        if (bob == 0) cout << "\n";
        cout << "\n";
        string scan = "  ";
        int d = f % 10;
        for (int dd = 0; dd < d; dd++) scan += FG_CYAN + ". " + RST;
        cout << scan << "\n\n";
        string wv; int wOff = f % 6;
        for (int w = 0; w < 22; w++)
            wv += ((w+wOff)%4 < 2) ? FG_LTBLUE+"~  "+RST : FG_CYAN+" ~ "+RST;
        cout << "  " << wv << "\n\n";
        cout << FG_ORANGE << "  Searching for resources...\n" << RST;
        cout << flush;
        sleepMs(180);
    }
    sleepMs(200);
}

// ============================================================================
// REST ANIMATION
// ============================================================================

void Display::restAnimation() {
    cout << "\033[2J\033[H";
    static const char* ANCHOR[8] = {
        "       _____     ",
        "      /     \\    ",
        "      |  O  |    ",
        "      |     |    ",
        "  ----+-----+--- ",
        "      |  |  |    ",
        "     (|  |  |)   ",
        "      |__|__|    "
    };
    for (int f = 0; f < 14; f++) {
        cout << "\033[H";
        cout << "\n\n" << FG_CYAN + BOLD
             << "  =====  CREW RESTING  =====  Stamina recovering...\n"
             << RST << "\n";
        int bob = (f % 4 < 2) ? 0 : 1;
        for (int blank = 0; blank < bob; blank++) cout << "\n";
        for (int r = 0; r < 8; r++)
            cout << "   " << FG_GOLD << ANCHOR[r] << RST << "\n";
        if (bob == 0) cout << "\n";
        cout << "\n";
        string wv; int wOff = f % 6;
        for (int w = 0; w < 22; w++)
            wv += ((w+wOff)%4 < 2) ? FG_LTBLUE+"~  "+RST : FG_CYAN+" ~ "+RST;
        cout << "  " << wv << "\n\n";
        int sc = f; if (sc > 10) sc = 10;
        string stars = "  ";
        for (int s = 0; s < sc; s++) stars += FG_GOLD + "* " + RST;
        cout << stars << "\n";
        cout << FG_GREEN << "  Stamina +3   Sanity +5   Consecutive sailing reset\n" << RST;
        cout << flush;
        sleepMs(200);
    }
    sleepMs(300);
}

// ============================================================================
// STORM ANIMATION
// ============================================================================

void Display::stormAnimation() {
    cout << "\033[2J\033[H";
    for (int f = 0; f < 12; f++) {
        cout << "\033[H";
        bool lightning = (f % 3 == 0);
        string cc = lightning ? FG_WHITE + BOLD : FG_GRAY;
        cout << "\n\n";
        cout << cc << "  ~~###~~~###~~~###~~~###~~~###~~~###~~~###~~\n"
                   << "~###########################################~\n" << RST;
        cout << "\n" << FG_RED+BOLD
             << "     !! SUDDEN STORM !!  Brace for impact...\n" << RST << "\n";
        if (lightning) {
            cout << FG_GOLD+BOLD
                 << "          /\\\n"
                 << "         /  \\\n"
                 << "        / /\\ \\\n"
                 << "       /_/  \\_\\\n" << RST;
        } else {
            cout << "\n\n\n\n";
        }
        cout << "\n";
        string wv1, wv2; int wOff = f % 3;
        for (int w = 0; w < 22; w++) {
            bool a = ((w+wOff)%2==0);
            wv1 += a ? FG_CYAN+"~^~"+RST   : FG_BLUE+"^~^"+RST;
            wv2 += a ? FG_BLUE+"^~^"+RST   : FG_LTBLUE+"~^~"+RST;
        }
        cout << "  " << wv1 << "\n" << "  " << wv2 << "\n" << "  " << wv1 << "\n";
        cout << flush;
        sleepMs(lightning ? 80 : 150);
    }
    sleepMs(300);
}

// ============================================================================
// EVENT BANNER
// ============================================================================

void Display::showEventBanner(const string& eventName, bool isGood) {
    string color = isGood ? FG_LTBLUE : FG_RED;
    string label = isGood ? "  GOOD EVENT  " : "  BAD EVENT   ";
    string icon  = isGood ? "  **" : "  !!";
    const int W  = 54;
    int nameLen  = (int)eventName.size();
    int padLeft  = (W - nameLen) / 2; if (padLeft  < 0) padLeft  = 0;
    int padRight = W - padLeft - nameLen; if (padRight < 0) padRight = 0;
    cout << "\n" << color + BOLD;
    cout << "  +" << string(W,'=') << "+\n";
    cout << "  |" << icon << "  " << label
         << string(W - (int)label.size() - 4, ' ') << "|\n";
    cout << "  |" << string(padLeft,' ') << eventName
         << string(padRight,' ') << "|\n";
    cout << "  +" << string(W,'=') << "+\n";
    cout << RST << "\n";
}

// ============================================================================
// PORT BANNER
// ============================================================================

void Display::showPortBanner(int portKm) {
    cout << "\n" << FG_GOLD + BOLD;
    cout << "  +====================================================+\n";
    string content = "  WELCOME TO PORT  (" + to_string(portKm) + " km)";
    int innerWidth = 52;
    int contentLen = (int)content.size();
    int leftPad  = (innerWidth - contentLen) / 2;
    int rightPad = innerWidth - contentLen - leftPad;
    if (leftPad  < 0) leftPad  = 0;
    if (rightPad < 0) rightPad = 0;
    cout << "  ||" << string(leftPad, ' ') << content
         << string(rightPad, ' ') << "||\n";
    cout << "  ||  Stock up on supplies and repair your vessel.    ||\n";
    cout << "  +====================================================+\n";
    cout << RST << "\n";
}

// ============================================================================
// PET DEATH NOTICE
// ============================================================================

void Display::showPetDeathNotice(const string& petName) {
    cout << "\n" << FG_RED + BOLD;
    cout << "  +====================================================+\n";
    cout << "  ||                                                  ||\n";
    string line1 = petName + " has passed away...";
    string line2 = "You feel a deep and lasting sadness.";
    cout << "  ||   " << FG_WHITE << padTo(line1, 46) << FG_RED << "  ||\n";
    cout << "  ||   " << FG_WHITE << padTo(line2, 46) << FG_RED << "  ||\n";
    cout << "  ||                                                  ||\n";
    cout << "  +====================================================+\n";
    cout << RST << "\n";
}

// ============================================================================
// VICTORY SCREEN
// ============================================================================

void Display::victoryScreen(int daysUsed, bool bff) {
    static const char* FLAG0[5] = {
        "    o              ",
        "    | \\=========\\ ",
        "    |   YOKOHAMA   ",
        "    | /=========/  ",
        "    |              "
    };
    static const char* FLAG1[5] = {
        "    o               ",
        "    |  \\=========\\ ",
        "    |   YOKOHAMA    ",
        "    |  /=========/  ",
        "    |               "
    };
    static const char* VSHIP[4] = {
        "      |      ",
        "     /|\\     ",
        "    / | \\    ",
        "###/##|##\\###"
    };
    cout << "\033[2J\033[H";
    for (int f = 0; f < 10; f++) {
        cout << "\033[H";
        int daysDigits = (int)to_string(daysUsed).size();
        int flagPad = 27 - daysDigits;
        if (flagPad < 0) flagPad = 0;
        cout << "\n" << FG_GOLD + BOLD;
        cout << "  +============================================================+\n";
        cout << "  ||                                                          ||\n";
        cout << "  ||       Y O K O H A M A   R E A C H E D !                ||\n";
        cout << "  ||                                                          ||\n";
        cout << "  ||           3000 km  --  VOYAGE COMPLETE                  ||\n";
        cout << "  ||           Days at sea: " << FG_WHITE << daysUsed
             << FG_GOLD << string(flagPad,' ') << "||\n";
        cout << "  ||                                                          ||\n";
        cout << "  +============================================================+\n";
        cout << RST << "\n";
        for (int r = 0; r < 4; r++)
            cout << "         " << FG_BROWN << VSHIP[r] << RST << "\n";
        cout << "\n";
        const char** flag = (f%2==0) ? FLAG0 : FLAG1;
        cout << "         " << FG_WHITE << flag[0] << RST << "\n";
        for (int r = 1; r < 4; r++)
            cout << "         " << FG_ORANGE+BOLD << flag[r] << RST << "\n";
        cout << "         " << FG_WHITE << flag[4] << RST << "\n\n";
        string wv;
        for (int w = 0; w < 28; w++)
            wv += ((w+f)%3==0) ? FG_BLUE+"~^"+RST : FG_CYAN+"^^"+RST;
        cout << "  " << wv << "\n\n";
        cout << FG_WHITE << "  Congratulations, Captain! Your crew survived the voyage.\n" << RST;
        if (bff)
            cout << FG_MAG+BOLD << "  *** ACHIEVEMENT: Best Friend Forever! ***\n" << RST;
        cout << flush;
        sleepMs(260);
    }
}

// ============================================================================
// DEFEAT SCREEN
// ============================================================================

void Display::defeatScreen(const string& reason) {
    cout << "\033[2J\033[H";
    static const char* SHIP_SINK[4] = {
        "    /\\  /\\    ",
        "   /  \\/  \\   ",
        "  / VOYAGE \\  ",
        " /___________\\"
    };
    for (int f = 0; f < 8; f++) {
        cout << "\033[H";
        cout << "\n" << FG_RED+BOLD;
        cout << "  +============================================================+\n";
        cout << "  ||                                                          ||\n";
        cout << "  ||              V O Y A G E   F A I L E D                  ||\n";
        cout << "  ||                                                          ||\n";
        string pr = reason;
        while ((int)pr.size() < 54) pr += ' ';
        if ((int)pr.size() > 54) pr = pr.substr(0,54);
        cout << "  ||   " << pr << "   ||\n";
        cout << "  ||                                                          ||\n";
        cout << "  +============================================================+\n";
        cout << RST << "\n";
        int sinkDepth = f/2;
        for (int blank = 0; blank < sinkDepth; blank++) cout << "\n";
        for (int r = sinkDepth; r < 4; r++)
            cout << "      " << FG_BROWN << SHIP_SINK[r] << RST << "\n";
        int wl = f*6; if (wl > 50) wl = 50;
        cout << "  " << FG_BLUE+BOLD+string(wl,'~')+RST << "\n\n";
        cout << FG_RED << "  The sea has claimed your vessel...\n" << RST;
        cout << flush;
        sleepMs(300);
    }
}