# 🚢 **ABYSSAL ODYSSEY: THE YOKOHAMA QUEST** 🚢
### An Epic Survival Adventure
**COMP2113 / ENGG1340 Course Project 2025–2026 2nd Semester**

![C++](https://img.shields.io/badge/C%2B%2B-Project-blue?style=for-the-badge)
![Text-Based](https://img.shields.io/badge/Text--Based-Game-green?style=for-the-badge)
![Linux](https://img.shields.io/badge/Linux-Terminal-orange?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-1.0-purple?style=for-the-badge)

---

## 🧭 Table of Contents
1. [Team Members](#team-members)
2. [3-Minute Demo Video](#3-minute-demo-video)
3. [Game Description](#game-description)
4. [Coding Requirements Satisfied](#coding-requirements-satisfied)
5. [Additional Features](#additional-features)
6. [Non‑Standard Libraries](#non-standard-libraries)
7. [Compilation & Execution Instructions](#compilation--execution-instructions)
8. [How to Play](#how-to-play)
9. [Project Structure](#project-structure)
10. [License](#license)

---

## 👥 Team Members
| Member | UID | Role & Responsibility |
|--------|-----|------------------------|
| Li Aitong | 3036588060 | Team Leader / Main Program & Integration + Video |
| Yau Isaac Sung-yip | 3036587884 | Core Game Logic (Resources, Actions, Status) |
| Shu Siming | 3036484771 | Random Event System + Difficulty + Loot Tables + UI Display|
| Zhao Ryan En Han | 3036460294 | Port System + Save/Load (File I/O) |
| Yu Ho Kwan Marcus | 3036636453 | UI, Input & Pet System |

---

## 🎥 3-Minute Demo Video
Link: https://youtu.be/YilnJM0iFvc?feature=shared


---

## 🌊 Game Description
🌊 Welcome to Abyssal Odyssey: Ocean Voyage Survival Challenge, a gripping text-based survival game built entirely in C++! We’re thrilled to bring the thrill of high-seas adventure to life in a strategic, immersive way. In this game, you’ll step into the role of captain, leading a 3000km voyage from Hong Kong to Yokohama. Your mission: master resource management, outsmart random ocean events, and guide your crew, ship, and loyal pet companion to safety before the deadline!

⚓ At the start of your journey, you’ll set sail with your crew and a trusty pet, ready to face the unpredictable Pacific. Every day, you’ll choose your action: push forward with sailing to make progress, explore for rare supplies and treasures, or rest to restore your crew’s stamina and sanity.

🌪️ As you sail, you’ll encounter dynamic random events—from devastating storms and supply shortages to lucky discoveries and friendly traders. Each choice you make carries real consequences: mismanage your food, water, or ship durability, and your voyage could end in disaster.

🛍️ When you reach coastal ports, you’ll have the chance to trade for vital provisions, repair your damaged ship, and stock up for the long journey ahead. Keep a close eye on your resources, your crew’s morale, and the ticking deadline to stay on course.

🏆 Your ultimate goal is to reach Yokohama safely before time runs out, keeping your crew, ship, and pet alive through every storm and crisis. Only the most strategic, quick-thinking captains will conquer the Pacific and complete their voyage!

🎮 Our game is tense, strategic, and endlessly replayable, with multiple difficulty levels and a deep pet companion system. With its engaging decision-driven gameplay and nostalgic text-adventure feel, we’re sure it will appeal to survival game fans and strategy lovers alike. So hoist the sails, and set off on your ocean odyssey!

---

## ✅ Coding Requirements Satisfied
This project fully implements **all 6 mandatory coding elements** required by the course:

1. **Generation of random events**
   - Random events trigger after Sail / Explore
   - Good/bad probabilities depend on difficulty, sanity, and fatigue
   - Random rewards, penalties, loot, and port encounters

2. **Data structures for storing game data**
   - Structs: `GameState`, `Resource`, `Crew`, `Ship`, `Pet`
   - Structured storage for all game status, resources, and progress

3. **Dynamic memory management**
   - Dynamic allocation for game data and save slots
   - Safe memory deallocation to avoid memory leaks

4. **File input/output (loading/saving game)**
   - `saveGame()` and `loadGame()` functions
   - Persistent progress stored in text files
   - Save/load available inside ports

5. **Program codes in multiple files**
   - Modular design across multiple `.h` / `.cpp` files
   - Clear separation: main, logic, events, port, save, UI, pet

6. **Multiple difficulty levels**
   - Story, Easy, Medium, Hard
   - Different deadlines, day limits, event rates, port numbers, price multipliers

---

## 🚀 Additional Features
🎮 **Core Daily Gameplay**
- Three Meaningful Daily Actions: Choose between Sail (progress toward your destination), Explore (scavenge rare supplies and treasures), Rest (restore crew stamina and sanity), or interact with pets — every choice shapes your voyage’s outcome.
  
🧠 **Immersive Survival Systems**
- Dynamic Fatigue System: Consecutive sailing inflicts escalating stamina penalties, forcing strategic rest to avoid crew exhaustion and catastrophic failures.
- Sanity Mechanic: Crew sanity directly modifies random event probabilities — low sanity drastically increases the risk of deadly storms, mutinies, and other crises, while high sanity unlocks rare beneficial encounters.
  
🐾 **Pet Companion System**
- Full Pet Lifecycle Management: Nurture your loyal companion with food, care, and affection to maintain its mood. Unlock the exclusive Best Friend Forever achievement by keeping your pet healthy and happy for the entire voyage, with unique pet-triggered events and bonuses.
  
🏙️ **Port & Progression Features**
- Comprehensive Port System: Trade for vital supplies, repair ship damage, save your progress, and restock at coastal stops — critical for surviving the 3000km Pacific crossing.
  
🎯 **Endgame & Replayability**
- Robust Win/Lose Conditions: Multiple distinct failure endings (crew starvation, ship sinking, pet death, missed deadline) and a single true victory condition, with balanced difficulty scaling for endless replayability.

---

## 📚 Non‑Standard Libraries
- **No non‑standard or external libraries used**
- All code uses only standard C/C++ libraries pre‑installed on the department server
- No extra installation or downloads required

---

## 🔧 Compilation & Execution Instructions
### Prerequisites
- Linux terminal (Academy server supported)
- g++ compiler
- make

### Compile
```bash
make
```

### Run the game
```bash
./ocean_voyage
```

### Clean build files
```bash
make clean
```

---

## 🎮 How to Play
> **Reminder** : The recommended terminal size is 55x125. You can still play at any size, but the UI may not align perfectly.
1. Select your preferred difficulty
2. Choose whether to bring a pet
3. Each day: select **Sail / Explore / Rest**
4. Respond to random events
5. Visit ports to trade, repair, and save
6. Reach **3000 km** before the deadline to win!

---

## 📁 Project Structure
```
.
├── main.cpp              # Program entry & main game loop
├── game.h / game.cpp     # Core game logic & state management
├── event.h / event.cpp   # Random events, difficulty, loot tables
├── port.h / port.cpp     # Port system & trading
├── save.h / save.cpp     # File I/O, save/load functions
├── ui.h / ui.cpp         # UI menus, input handling
├── ui_display.h / ui_display.cpp  # UI rendering & display logic
├── pet.h / pet.cpp       # Pet system & achievement
├── Makefile              # Compilation script
├── README.md             # Project documentation
└── save_*.txt            # Save files (generated at runtime)
```

---

## 📄 License
This project is for **educational purposes only** as part of  
**COMP2113 / ENGG1340 2025–2026 2nd Semester** at The University of Hong Kong.

---
