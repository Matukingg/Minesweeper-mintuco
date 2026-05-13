# Buscaminas

A Minesweeper clone written in C++ using the Allegro 5 graphics library.

## Features

- Classic Minesweeper gameplay on a configurable grid
- Left-click to reveal, right-click to place/remove a flag
- Flood-fill: clicking an empty cell auto-reveals all connected empty cells
- Chord-click: clicking a revealed number auto-reveals its neighbors when the correct number of flags are placed
- First-click safety: the map generates on your first click, guaranteeing a zero (empty region) at that spot
- Mine counter and elapsed timer in the toolbar
- Reset button (the face) restarts the game at any time

## Controls

| Input | Action |
|---|---|
| Left click | Reveal cell |
| Right click | Place / remove flag |
| Left click (on number) | Chord reveal (if flags match the number) |
| Left click (reset button) | New game |

## Building

Requires **Allegro 5** and **Premake4**.

```bash
# Install dependencies (Debian/Ubuntu)
sudo apt install liballegro5-dev premake4

# Build (debug)
bash create.sh

# Run
bash run.sh
```

## Configuration

`settings.txt` is auto-generated on first run. Edit it to change difficulty:

```
grid_width=10
grid_height=10
num_mines=15
fullscreen=0
```

## Project Structure

```
src/
  main.cpp           — game loop and event handling
  core/              — Allegro init, settings load/save (singleton)
  utils/             — Map: grid state, mine generation, reveal logic
  tiles/             — Bitmaps: tile image loading
  graphics/          — Graphic_Manager: display and rendering
  input/             — Input: Allegro event queue
data/                — PNG tile assets
assets/              — Source art files (.xcf)
```
