# Buscaminas — Claude Context

## Tech Stack

- **Language:** C++17
- **Graphics:** Allegro 5 (`allegro`, `allegro_image`, `allegro_font`, `allegro_primitives`)
- **Build:** Premake4 → GNU Make. Output lands in `build/bin/debug/buscaminas`.
- **Build command:** `bash create.sh` (cleans, runs premake4, then make debug)
- **Run command:** `bash run.sh`

## Architecture

| File | Responsibility |
|---|---|
| `src/main.cpp` | Game loop, event dispatch, timer, first-click logic |
| `src/core/core.hpp/.cpp` | Allegro init/shutdown singleton, settings load/save (`settings.txt`) |
| `src/utils/map.hpp/.cpp` | Grid data, mine generation (first-click safe), reveal, chord, flood-fill, win check |
| `src/tiles/bitmaps.hpp/.cpp` | Loads PNG tiles from `data/`; exposes getters per tile type |
| `src/graphics/graphicMgr.hpp/.cpp` | Creates display, renders toolbar (mine counter, timer, reset button) and grid |
| `src/input/input.hpp/.cpp` | Owns `ALLEGRO_EVENT_QUEUE`, registers display + mouse sources |

## Key Design Decisions

- **Map generates on first click**, not at startup. `Map::generateMap(mines, safe_x, safe_y)` excludes the 3×3 zone around the clicked cell from mine placement, guaranteeing a 0 there.
- **Event-driven loop** — no fixed timestep. The display only redraws on mouse events or the 1-second `ALLEGRO_TIMER` tick.
- **Core is a singleton** (`Core::instance()`). Do not construct a `Core` on the stack in `main` — Allegro would be double-initialized.
- **Toolbar height** is `Graphic_Manager::TOOLBAR_H = 40px`. Grid click Y coordinates must subtract this offset before converting to cell coordinates.
- **Tile path** in `bitmaps.cpp` is hardcoded to an absolute path (`/home/matuco/Documents/minesweeper/data/`). If the project moves, update that constant.

## Settings

`settings.txt` (auto-generated, gitignored):
```
grid_width=10
grid_height=10
num_mines=15
fullscreen=0
difficulty=normal
```

## Known Rough Edges

- Win/loss overlay text uses Allegro's built-in bitmap font (8px) — small but readable.
- No keyboard shortcut to reset (only the toolbar button).
- Tile path is not relative to the executable.
