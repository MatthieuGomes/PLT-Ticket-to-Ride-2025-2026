# Terminal Interface (TUI) description

## Overview
The terminal UI (TUI) package is a custom renderer built on ANSI escape codes. It is composed of a `Terminal` backend object plus several framed views managed by the interface controller `TUIManager` (`src/shared/tui/Terminal.cpp`, `src/shared/tui/TUIManager.cpp`). The goal is to keep the interface simple and fully driven by the game state.

## Core architecture
- **Terminal**: Low-level ANSI writer with codes (colors, cursor control, clear, flush) and basic character control methods. See `src/shared/tui/Terminal.cpp` and `src/shared/tui/Terminal.h`.
- **BaseView**: Common frame drawing class and title logic for all subviews with prototype screen operation functions overriden by its realizations (`src/shared/tui/BaseView.cpp`).
- **StatusBar**: Title bar subframe containing game title + version + turn (or RENDER_DEBUG text if in render view). See `src/shared/tui/StatusBar.cpp`.
- **GameView**: Main content area that switches between Map / Player / Cards views (`src/shared/tui/GameView.cpp`).
- **InfoPanel**: Scrollable log of events and messages (`src/shared/tui/InfoPanel.cpp`).
- **CommandInput**: Single-line command entry with input length cap (`src/shared/tui/CommandInput.cpp`).
- **TUIManager**: Controls Layout, focus, input loop, and dispatch, along with information transmission between terminal and engine (`src/shared/tui/TUIManager.cpp`).

## Layout and sizing
- Screen is split into three vertical blocks: StatusBar (top), content area (middle), CommandInput (bottom). The content area is further split into GameView and InfoPanel (`src/shared/tui/TUIManager.cpp`).
- TUI adapts to terminal size by recomputing subview sizes via `updateLayout` (`src/shared/tui/TUIManager.cpp`). No dynamic redimensioning has been implemented for now.

## Input model and focus
- **Focus cycle**: `Tab` moves focus forward and `Shift+Tab` backward among CommandInput, GameView, and InfoPanel (`src/shared/tui/TUIManager.cpp`).
- **GameView**: With focus, arrow keys move selection across the station map grid on a next line/row neghbour basis; typing letters builds a search query and matches text to the station name to select it. `Enter` clears the search and backspace edits it (`src/shared/tui/TUIManager.cpp`, `src/shared/tui/GameView.cpp`). Map is scrollable if the game is started in a terminal smaller than the space needed to fully render it at once.
- **InfoPanel**: Prints all general game information to the player (results of his/other players' actions and others). `PgUp/PgDn` scroll by page, while arrows scroll line-by-line when InfoPanel has focus (`src/shared/tui/TUIManager.cpp`).
- **CommandInput**: Printable chars are accepted up to a limit of 50 (currently checked by polling), backspace erasing supported, Enter submits (`src/shared/tui/TUIManager.cpp`, `src/shared/tui/CommandInput.cpp`) the input.
- **View switching**: While GameView has focus, `Shift+m` selects Map view, `shift+p` selects Player view, and `shift+c` selects Cards view (only in render debug) (`src/shared/tui/TUIManager.cpp`).

## Map view
- Stations are rendered on a 2D grid using `TUIrow`, `TUIcolumn`, and `TUInickname` from the state JSON (`src/shared/tui/GameView.cpp`, `static/europe_state.json`).
- The map auto-scrolls to keep the selected station in view. The last line of the map area shows a search prompt.
- A details panel shows the selected station, its owner, and a compact road table with type, endpoints, length, color, owner, and ferry locomotive count if applicable (`src/shared/tui/GameView.cpp`).

## Player view
- Displays the local player with full detail (hand, destination cards, score, wagons, stations), while other players are shown only as wagons/score/stations. In render debug mode, all players show full details (`src/shared/tui/GameView.cpp`).
- The right column list supports horizontal scrolling to display more than the visible number of players (`src/shared/tui/GameView.cpp`).

## Cards view (render debug)
- Cards view is available in render debug to visualize the entire shared deck and player hands for testing (`src/shared/tui/GameView.cpp`, `src/shared/render/RenderState.cpp`).

## Command parsing
- Commands typed in CommandInput are collected by TUIManager via TUIManager::HandleInput() and transformed into a JSON payload (currently `{"action":"...","args":"..."}`) via `tui::CommandParser` (`src/shared/tui/CommandParser.cpp`). 
Currently, TUIManager itself re-parses the created JSON with the helper `TUIManager::extractAction()` to detect if the 'exit' action was sent and exit the program. Other commands will be sent to the engine and TUIManager will act according to the answer sent by it.

## Data sources
- The TUI reads actual state objects provided by `state::State` when launched with `./bin/client tui` (`src/client/main.cpp`, `src/shared/state/State.cpp`).
- Map layout is loaded from the same JSON state file used for rendering, controlled via the `TUI_LAYOUT_PATH` environment variable (`src/shared/tui/GameView.cpp`), using the `TUIrow`, `TUIcolumn` and `TUInickname` attributes to control column/row of printing and displayed name.

## Key files
- Manager and layout: `src/shared/tui/TUIManager.cpp`
- Map/player/cards rendering: `src/shared/tui/GameView.cpp`
- Status bar: `src/shared/tui/StatusBar.cpp`
- Info panel: `src/shared/tui/InfoPanel.cpp`
- Command input: `src/shared/tui/CommandInput.cpp`
- Terminal backend: `src/shared/tui/Terminal.cpp`
- Default JSON state: `static/europe_state.json`
