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
| Li Aitong | 3036588060 | Team Leader / Main Program & Integration |
| Yau Isaac Sung-yip | 3036587884 | Core Game Logic (Resources, Actions, Status) |
| Shu Siming | 3036484771 | Random Event System + Difficulty + Loot Tables |
| Zhao Ryan En Han | 3036460294 | Port System + Save/Load (File I/O) |
| Yu Ho Kwan Marcus | 3036636453 | UI, Input & Pet System |

---

## 🎥 3-Minute Demo Video
Link: https://youtu.be/YilnJM0iFvc?feature=shared


---

## 🌊 Game Description
**Ocean Voyage Survival Challenge** is a text‑based survival game set in the Pacific Ocean.  
You are the captain sailing **from Hong Kong to Yokohama (3000 km)**.

You must:
- Manage resources carefully
- Choose daily actions (Sail / Explore / Rest)
- Respond to random good/bad ocean events
- Trade supplies and repair your ship at ports
- Reach the destination **before the deadline**
- Keep your crew, ship, and pet alive

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
- Three daily actions: **Sail, Explore, Rest**
- **Fatigue System** with consecutive sailing penalties
- **Sanity System** that affects event probabilities
- **Pet System** with food, mood, and *Best Friend Forever* achievement
- **Port Trading**: buy supplies, repair ship, save game
- Complete **win/lose conditions** with multiple failure endings

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
├── ui.h / ui.cpp         # UI menus, input handling, display
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
