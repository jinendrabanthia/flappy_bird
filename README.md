# Flappy Bird (SFML 3)

A classic Flappy Bird clone built entirely in modern C++17 and SFML 3! 
Designed by Jinendra Banthia.

## Features
- **Procedural Audio**: No external audio files required! The flap and crash sound effects are generated mathematically at runtime.
- **Parallax Background**: Features a beautiful sky, procedural clouds, and a scrolling green forest floor.
- **Visual Haptics**: The bird sprite features a dynamic squash-and-stretch visual scale pop when jumping, simulating tactile feedback.
- **Persistent High Scores**: Your best score is saved directly to `highscore.txt` so it persists between gaming sessions.
- **Dynamic Scoring**: Includes an on-screen heads-up display tracking your score and survival time!

## Requirements
- CMake (3.10+)
- MinGW-w64 (or compatible C++17 compiler)
- SFML 3.0.2

## Build Instructions
1. Open a terminal in the root directory.
2. Generate build files:
   ```bash
   cmake -B build
   ```
3. Compile the game:
   ```bash
   cmake --build build
   ```
4. Run the executable found in the `build` directory!

## Controls
- **SPACE**: Start the game / Jump.
- **F**: Toggle Anti-Gravity Mode (bonus feature!).
