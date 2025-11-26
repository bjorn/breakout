# Breakout

A simple breakout game - using a particle engine<br>
by Bjørn Lindeijer

## Status

Needless to say, this game is not finished. It lacks many things like sounds, effects, graphics, main menu, end of game and powerups. But it also lacks decent collision handling (errors on colliding with two blocks at the same time) and it doesn't have Z-ordering. To fix the last two things my particle engine would require another major rewrite, and I might have to create a more treelike designed game instead of using linked lists all the time.

Nevertheless, the game is playable. The first level might be a bit boring (coins were supposed to scatter upon hitting a gold block), but it should be rewarding to play all three levels... and then pressing ESC to quit the game.

## Controls

- Release ball: Space
- Move pad left: Left Arrow
- Move pad right: Right Arrow
- Quit: Esc

## Building (CMake + SDL3)

This project is built using C++, CMake and SDL3.

### Prerequisites

- CMake 3.21+
- A C++17-compatible compiler (GCC/Clang/MSVC)
- SDL3 development libraries installed and discoverable by CMake

### Build steps

1. Create a build directory and configure:
   ```
   cmake -B build .
   ```
   If SDL3 isn’t found automatically, try:
   ```
   cmake -B build . -DCMAKE_PREFIX_PATH=/path/to/sdl3
   ```

2. Build:
   ```
   cmake --build build
   ```

3. Run:
   ```
   ./build/breakout
   ```

### Assets

All required assets (bitmaps, sounds, levels) are included in the repository. Ensure you run the game from the project root or copy the built executable next to the asset files.
