# Typespeed — Design Spec
_2026-05-14_

## Overview

A typing-speed practice game written in C++ with Allegro 5, following the same architecture as the sibling minesweeper project. Supports prose (English/Spanish) and code (Python, C++, LaTeX, HTML, JavaScript) categories, multiple round modes, and persistent paragraph progression for prose files.

---

## Architecture

Subdirectory module layout mirroring minesweeper:

```
src/
  main.cpp          — event loop, phase transitions, wires modules
  core/             — Allegro init/shutdown singleton + settings.txt load/save
  graphics/         — display, all rendering (menu, file picker, playing, results)
  input/            — ALLEGRO_EVENT_QUEUE, keyboard + display sources
  game/             — round state machine, character tracking, error logic, timer
  textbank/         — file scanning, paragraph/file loading, progress tracking
  stats/            — WPM calculation, accuracy, error count, history file, graph data
data/
  english/          — prose .txt files
  spanish/          — prose .txt files
  python/           — code .txt files
  cpp/              — code .txt files
  latex/            — code .txt files
  html/             — code .txt files
  javascript/       — code .txt files
  progress.txt      — paragraph index tracker (gitignored)
  history.txt       — per-category session history for graphs (gitignored)
```

`main.cpp` owns the `ALLEGRO_TIMER` and the top-level phase enum. All other state lives in the relevant module.

---

## Phases

```
MENU → CATEGORY → FILE_PICK → PREVIEW → PLAYING → RESULTS → (MENU or PLAYING again)
```

| Phase | Description |
|---|---|
| MENU | Pick mode: Paragraph, Time limit, Word count, Endless |
| CATEGORY | Pick category: English, Spanish, Python, C++, LaTeX, HTML, JavaScript |
| FILE_PICK | List of .txt files in the chosen category folder; shows file name |
| PREVIEW | Shows file name, paragraph/block number (prose) or full file info (code), word count, line count — confirm or go back |
| PLAYING | Main typing screen |
| RESULTS | WPM, accuracy %, error count, moment-to-moment WPM graph |

---

## Round Modes

| Mode | End condition |
|---|---|
| Paragraph | Finish the current passage (one paragraph or full code file) |
| Time limit | Clock hits zero; partially typed passage is scored |
| Word count | Target word count reached |
| Endless | User presses Escape; paragraphs/files stream continuously |

Mode and error handling (strict/lenient) are selected in MENU and persist to `settings.txt`.

---

## Playing Screen

- Full passage rendered as a block of text.
- Characters are colour-coded in real time:
  - **Neutral** — not yet reached
  - **Green** — correctly typed
  - **Red** — mistyped
- Current character has a blinking thin vertical bar cursor (like a text editor), driven by the Allegro timer tick.
- **Strict mode:** cannot advance past an error; must backspace to fix.
- **Lenient mode:** errors are marked red but typing continues; error count accumulates.
- A live WPM counter and elapsed time are shown in the toolbar.
- Escape exits to MENU (in non-endless modes) or stops the round (endless).

---

## Text Bank

### Prose (English / Spanish)

- Files are `.txt` files in `data/english/` or `data/spanish/`.
- Passages are **blank-line-separated blocks** (paragraphs).
- `data/progress.txt` tracks the current paragraph index per file (key=value format):
  ```
  english/1984.txt=3
  spanish/principito.txt=0
  ```
- On round start, the tracked paragraph is loaded and the index advances.
- When the last paragraph is finished, the file is marked `completed=1` and future opens pick a random paragraph.

### Code (Python, C++, LaTeX, HTML, JavaScript)

- Files are `.txt` files in their respective `data/` subfolder.
- The **full file** is loaded as the passage — no slicing.
- No progression tracking; a random file is suggested each time (user can pick another).

### Endless Mode

- User picks a category and file.
- For prose: paragraphs are served sequentially and loop (or pull randomly from the folder when exhausted).
- For code: the full file repeats, or a new random file from the folder is loaded after each completion.
- Round ends on Escape.

---

## Preview Screen

Shown before every round for every mode. Displays:

- File name
- For prose: "Paragraph N of M"
- For code: "Full file"
- Word count
- Line count
- \[Confirm\] and \[Pick different file\] buttons

---

## Stats & Results

Shown after every round (including endless):

| Stat | Description |
|---|---|
| WPM | (correctly typed words / elapsed minutes) |
| Accuracy % | correct keystrokes / total keystrokes |
| Error count | total mistyped characters |
| WPM graph | moment-to-moment WPM sampled every ~5 seconds; time axis scales to round length |

Session results are appended to `data/history.txt` for the per-category historical graph (same style as minesweeper leaderboard graph).

---

## Settings (`settings.txt`)

```
mode=paragraph
error_handling=strict
category=english
```

---

## Build System

Mirrors minesweeper exactly:

- `premake4.lua` — defines the solution and links Allegro 5 addons (`allegro`, `allegro_image`, `allegro_font`, `allegro_ttf`, `allegro_primitives`)
- `create.sh` — cleans, runs premake4, compiles debug
- `run.sh` — launches `build/bin/debug/typespeed`
- Output: `build/bin/debug/typespeed`

---

## Key Design Constraints

- Core is a singleton (`Core::instance()`). Do not construct on the stack.
- Event-driven loop — no fixed timestep. Redraws only on keyboard events or timer ticks.
- Tile/font paths should be relative to the executable, not hardcoded absolute paths (lesson from minesweeper).
- `data/progress.txt` and `data/history.txt` are gitignored — user-specific runtime state.
