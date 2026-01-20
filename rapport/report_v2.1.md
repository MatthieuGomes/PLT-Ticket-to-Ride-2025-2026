# Deliverable 2.1 — Render View

## Goal
The render view demonstrates that the game state can be rendered from data, not hardcoded logic. The same renderer shows different maps, players, and cards by loading a JSON file at startup.

## Rendering strategy (state-driven)
- Entry point is `./bin/client render` in `src/client/main.cpp`, which forwards to `client.callRender()`.
- The render package contains the class RenderState in `src/shared/render/RenderState.cpp`, a small aggregator of the render loop function `render::RenderState::renderStateLoop`, that constructs a `state::State` parsing the data from a specific JSON file given as argument, populates `MapState`, `PlayersState`, and `CardsState`, and wires it into the TUI (via `tui::TUIManager`).
- The map layout is also data-driven: station fields `TUIrow`, `TUIcolumn`, and `TUInickname` are read by `GameView` to position labels in a scrollable grid (`src/shared/tui/GameView.cpp`) in order to build the game map.

## Variability and launch options
- Default render: `./bin/client render` loads `static/europe_state.json`.
- Custom render: `./bin/client render --json /path/to/state.json` sets `RENDER_STATE_PATH` (and `TUI_LAYOUT_PATH`) to render a state based on the JSON file given as argument.
- Preset variants: `./bin/client render --state 1` or `--state 2` load `static/europe_state_1.json` or `static/europe_state_2.json` to show the evolution of a game by its 2 states at different points.
- All options are handled in `src/client/main.cpp` and the environment override is used in `src/shared/render/RenderState.cpp`.

## JSON format (overview)
The state JSON format consists of 4 main objects, each with their own properties:
### Player info (`players[]`): 
 - `name`, 
 - `color`, 
 - `score`, 
 - `nbWagons`, 
 - `nbStations`, 
 - `borrowedRoads` (road ids), 
 - `hand` (index into `cards.playerCards`).

### Stations (`map.stations[]`): 
- `name`, 
- `owner` (player name or null), 
- `TUIrow` (row in which the station is rendered in the TUI GameView section), 
- `TUIcolumn` (column in which the station is rendered in the TUI GameView section), 
- `TUInickname`(abbreviation by which the station is named in the TUI GameView section)

### Roads (`map.roads[]`): 
- `id`, 
- `stationA`, 
- `stationB`, 
- `length`, 
- `type` (Road, Ferry or Tunnel), 
- `color` (normal colors or UNKNOWN),
- `owner`, 
- `locomotives` (number for ferries).

### Game cards(`cards`): 
- `outOfGame`, 
- `faceUp`, 
- `faceDown`, 
- `playerCards` (object with destination and wagon card arrays)
- Reference examples: `static/state_template.json`, `static/dummy_state.json`
- Game implementations: `static/europe_state.json`, `static/europe_state_1.json`, `static/europe_state_2.json`.

## Debug render view
- Render mode uses the same TUI renderer, with a set flag(`TUIManager::setDebugRender(true)`) to show all players' data and the cards in game in the GameView subframe (`src/shared/render/RenderState.cpp`, `src/shared/tui/TUIManager.cpp`, `src/shared/tui/GameView.cpp`).

## Key source files
- Entry and CLI flags: `src/client/main.cpp`
- Render loop: `src/shared/render/RenderState.cpp`
- State parsing: `src/shared/state/State.cpp`
- Map / players / cards parsing: `src/shared/mapState/MapState.cpp`, `src/shared/playersState/PlayersState.cpp`, `src/shared/cardsState/CardsState.cpp`(ParseFronJSON() methods and jsoncpp helpers)
- TUI renderer (parts used): `src/shared/tui/TUIManager.cpp`, `src/shared/tui/GameView.cpp`
