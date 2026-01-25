# AI Controller (Current)

This document describes the current AI implementation. 
The AI logic is at this moment part of the engine package (`src/shared/engine/AIController.*`) but will be moved into its own `ai/` module and expanded later.

## 1) Location and Integration

- **Current location:** `src/shared/engine/AIController.h` / `src/shared/engine/AIController.cpp`
- **Integration point:** `engine::TurnContext` holds a list of `PlayerController` objects (`controllers`).
- **Assignment:** In `SetupState`, player 0 is set to `HumanController`; other players are assigned an `AIController`.

## 2) Functionality

The AI chooses a command based on the allowed commands exposed by the current `GameState`:

1. The engine enters a state (e.g., `PlayerTurnState`, `DrawWagonCardState`, `DestinationChoiceState`, `ConfirmationState`).
2. If the current player is AI, the state auto-requests a command from its controller.
3. `AIController::nextCommand()` inspects `currentState->getAllowedCommands()` and picks a supported command at random.
4. The command is encoded to JSON and immediately executed by the engine.
5. The state inserts an InfoPanel message such as:
   - `AI Player2: draw_faceup`
   - `AI Player3: select 1 destination(s)`

This produces a minimal, automated opponent that can advance the game without human input, thus allowing for solo runs of the game.

## 3) Commands the AI Can Produce

The AI filters out commands that should not be used:

- **Excluded:** `help`, `exit`
- **Included (if allowed by the current state):**
  - `draw_destination`
  - `select_destination` (auto-selects the required minimum)
  - `draw_faceup` or `draw_facedown` (respects draw-phase constraints)
  - `take` (uses first road id available)
  - `claim_station` (uses first station available)
  - `borrow`
  - `confirm`

If the state does not allow any usable command, the AI returns an empty command (the state does nothing).

## 4) Randomness and Determinism

- The AI currently uses a fixed seed by default in the `seed` attribute in `AIController`.
- This makes behavior repeatable for debugging; it will be varied later for more diversity.

## 5) Limitations of the random AI

- **No strategy:** The AI does not evaluate map, scores, or objectives like a human player would.
- **Simple selection:** Chooses the first visible road/station, and random allowed command.
- **No planning:** No pathfinding or ticket completion logic.
- **No awareness of game phase:** AI only obeys allowed command lists.

## 6) Planned Evolution

- Move AI to a dedicated `ai/` package.
- Add strategic planners (destination completion, longest path heuristics, scoring goals) with existing functions.
- Implement a decision tree based on the decided heuristics to improve AI decision quality.