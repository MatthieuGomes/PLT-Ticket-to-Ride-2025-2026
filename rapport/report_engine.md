# Engine State Machine (Current)

This document describes the current engine implementation (code + UML) and the runtime flow between TUI and engine.

## 1) Architecture Overview

The engine is a state machine that receives JSON commands from the TUI, validates them, applies game logic to the affected game modules, and returns JSON results with events/messages. The main data structures are:

- **Engine** (`src/shared/engine/Engine.h`, `src/shared/engine/Engine.cpp`)
  - Owns `state::State` and the current `GameState` via `StateMachine`.
  - Holds `TurnContext` (current player, draw phase data, pending tunnel/tickets, last round flags).
  - Stores `pendingEvents` for out-of-band messages (InfoPanel).
- **StateMachine** (`src/shared/engine/StateMachine.h`, `src/shared/engine/StateMachine.cpp`)
  - Holds `currentState`.
  - `transitionTo()` runs `onExit()` then `onEnter()` of the next state.
  - `handleCommand()` delegates to the current state.
- **GameState** base class (`src/shared/engine/GameState.h`, `src/shared/engine/GameState.cpp`)
  - Virtual `onEnter()`, `handleCommand()`, `getAllowedCommands()`.
- **CommandParser** (`src/shared/engine/CommandParser.h`, `src/shared/engine/CommandParser.cpp`)
  - Parses JSON to `EngineCommand`, then delegates to the StateMachine.
  - Builds `EngineResult` to JSON and adds `EngineEvent` messages.
- **PlayerController** hierarchy (`src/shared/engine/PlayerController.h`, `HumanController`, `AIController`)
  - Determines how commands are produced (human via TUI, AI via auto-play).
  - The engine uses controllers stored in `TurnContext`.

## 2) JSON Command/Result Flow (TUI <-> Engine)

**TUI -> Engine**
1. `tui::CommandInput` gathers user text.
2. `tui::CommandParser::parse()` turns text into a JSON command string.
3. `TUIManager::handleInput()` sends JSON to `engine::CommandParser::parseAndApply()`.
4. The engine returns a JSON result containing `events[]` and `payload.exit`.

**Engine -> TUI**
1. `TUIManager` parses the JSON result with `parser::JSONParser::parseResult()`.
2. Each `EventMessage.message` is appended to InfoPanel.
3. If `payload.exit == true`, TUI exits.

Files:
- TUI parsing + routing: `src/shared/tui/TUIManager.cpp`, `src/shared/tui/CommandParser.cpp`.
- Engine command parsing: `src/shared/engine/CommandParser.cpp`.
- JSON serialization: `src/shared/parser/JSONParser.cpp`.

## 3) Core Engine Data Structures

**TurnContext** (`src/shared/engine/TurnContext.h`)
- `currentPlayer`: active player index
- `drawsRemaining`, `drawSource`: wagon draw tracking
- `finalRound`, `finalRoundStarter`: last-round logic
- `pendingTickets`: temporary destination tickets
- `pendingTunnel`: tunnel claim tracking
- `controllers`: list of `PlayerController` for AI/human

**EngineResult / EngineEvent** (`src/shared/engine/EngineResult.h`, `EngineEvent.h`)
- Engine states return `EngineResult` with `ok`, `error`, `nextPhase`, `events`.
- Events are surfaced to the TUI InfoPanel.

**PendingTickets / PendingTunnel**
- `PendingTickets` stores drawn destination cards before selection.
- `PendingTunnel` stores tunnel claim data (route, base length, color, extraRequired, revealed cards).

## 4) State Machine: Current State Logic

### SetupState
- Creates the default game state or uses provided `state::State`.
- Initializes `TurnContext` (players/controllers).
- Transitions to `PlayerAnnounceState`.
+Files: `src/shared/engine/SetupState.cpp`

### PlayerAnnounceState
- Announces the active player via `EngineEvent`.
- Also checks final round condition early (used for testing).
- Transitions to `PlayerTurnState`.
+Files: `src/shared/engine/PlayerAnnounceState.cpp`

### PlayerTurnState
- Main hub for player commands.
- Allowed commands: draw destination, draw wagon faceup/facedown, claim road, claim station, borrow road, exit.
- For AI controllers, auto-sends one command immediately on enter.
+Files: `src/shared/engine/PlayerTurnState.cpp`

### DrawDestinationCardState
- Draws destination tickets from deck; handles cases with 1–3 cards left.
- Sets `pendingTickets` and `minKeepTickets`.
- If only one card remains, auto-add to player and return to PlayerTurn.
- Otherwise transitions to DestinationChoiceState.
+Files: `src/shared/engine/DrawDestinationCardState.cpp`

### DestinationChoiceState
- Player selects which destination cards to keep (JSON payload `indices`).
- Must keep at least `minKeepTickets`.
- Unselected cards return to destination deck.
- Transitions to ConfirmationState.
+Files: `src/shared/engine/DestinationChoiceState.cpp`

### DrawWagonCardState
- Controls draw phase (2 cards max).
- Routes to FaceUpState or FaceDownState based on command.
- AI auto-plays a draw (honors source constraints).
+Files: `src/shared/engine/DrawWagonCardState.cpp`

### FaceUpState
- Draws a selected face-up card.
- If locomotive drawn first, draw phase ends immediately.
- Enforces “same source” rule (only face-up once started).
+Files: `src/shared/engine/FaceUpState.cpp`

### FaceDownState
- Draws from face-down pile.
- If face-down draw is chosen, draws 2 cards at once and ends phase.
+Files: `src/shared/engine/FaceDownState.cpp`

### ClaimRoadState
- Parses road id and color choice.
- Validates ownership, availability, and card counts.
- If tunnel, transitions to TunnelResolveState.
- Else transitions to RoadResolveState.
+Files: `src/shared/engine/ClaimRoadState.cpp`

### TunnelResolveState
- Tunnel claim flow:
  - Player selects color.
  - Draws 3 extra cards, increases cost if matching.
  - Checks if player can pay (color + locomotives).
  - If fail, returns cards to hand and ends.
  - If success, removes cards and continues.
- Transition: RoadResolveState.
+Files: `src/shared/engine/TunnelResolveState.cpp`

### RoadResolveState
- Applies road ownership, wagon deduction, score update.
- Transition: ConfirmationState.
+Files: `src/shared/engine/RoadResolveState.cpp`

### ClaimStationState
- Allows building a station on an unowned station.
- Cost depends on remaining station pieces (3→cost1, 2→cost2, 1→cost3).
- Adds station ownership and updates player resources.
- Transition: ConfirmationState.
+Files: `src/shared/engine/ClaimStationState.cpp`

### BorrowRoadState
- Validates a road owned by another player.
- Registers borrowed road for the active player.
- Transition: ConfirmationState.
+Files: `src/shared/engine/BorrowRoadState.cpp`

### ConfirmationState
- End-of-turn gate; only allows `confirm` or `borrow`.
- For AI, auto-confirms.
- Transition: EndTurnState or BorrowRoadState.
+Files: `src/shared/engine/ConfirmationState.cpp`

### EndTurnState
- Advances `currentPlayer`.
- Triggers final round if any player drops to 2 wagons.
- If last round ends, transitions to EndGameState.
- Otherwise goes to PlayerAnnounceState (or WaitTurn for AI-controlled players).
+Files: `src/shared/engine/EndTurnState.cpp`

### WaitTurnState
- Used for non-active players (AI step between turns).
- Logs “Waiting for other players...”, then immediately transitions to PlayerAnnounceState.
- Only `exit` allowed.
+Files: `src/shared/engine/WaitTurnState.cpp`

### EndGameState
- Computes final scores (including destination completion).
- Announces winner.
- Transitions to ExitState on player command.
+Files: `src/shared/engine/EndGameState.cpp`

### ExitState
- Confirms exit to TUI via EngineResult.
- Engine is destroyed by TUI after exit confirmation.
+Files: `src/shared/engine/ExitState.cpp`

## 5) Command Set (Current)

Commands are JSON-based, with `name` and optional `payload`:

- `exit`
- `draw_destination`
- `select_destination` (payload: `indices`)
- `draw_faceup` (payload: `index`)
- `draw_facedown`
- `take` (payload: `roadId`, optional `color`)
- `claim_station` (payload: `station`)
- `borrow` (payload: `roadId`)
- `confirm`
- `color` (tunnel color selection)

Parsing is implemented in `src/shared/engine/CommandParser.cpp`.

## 6) Player / AI Control Flow

**PlayerController** provides:
- `nextCommand(state, engine)`
- `notify(result)`

**HumanController**
- Pass-through for now; the TUI provides commands.

**AIController**
- Chooses from `currentState->getAllowedCommands()` (filters help/exit).
- Builds a JSON command (e.g., draw or claim).
- The engine auto-executes the command in `PlayerTurnState`/draw/choice/confirmation states.

## 7) Transition Table (Summary)

| From State | Trigger / Condition | To State |
|-----------|---------------------|---------|
| Setup | Done | PlayerAnnounce |
| PlayerAnnounce | Always | PlayerTurn |
| PlayerTurn | draw_destination | DrawDestinationCard |
| PlayerTurn | draw_faceup/draw_facedown | DrawWagonCard |
| DrawWagonCard | draw_faceup | FaceUp |
| DrawWagonCard | draw_facedown | FaceDown |
| FaceUp | Draw complete | Confirmation |
| FaceDown | Draw complete | Confirmation |
| PlayerTurn | take | ClaimRoad |
| ClaimRoad | Tunnel | TunnelResolve |
| ClaimRoad | Non-tunnel | RoadResolve |
| RoadResolve | Applied | Confirmation |
| TunnelResolve | Resolved | RoadResolve |
| PlayerTurn | claim_station | ClaimStation |
| ClaimStation | Applied | Confirmation |
| PlayerTurn | borrow | BorrowRoad |
| BorrowRoad | Applied | Confirmation |
| DrawDestinationCard | Draw complete | DestinationChoice |
| DestinationChoice | select_destination | Confirmation |
| Confirmation | confirm | EndTurn |
| Confirmation | borrow | BorrowRoad |
| EndTurn | Next player | PlayerAnnounce / WaitTurn |
| EndTurn | Last round complete | EndGame |
| EndGame | Done | Exit |
| Exit | Done | (TUI exits) |
