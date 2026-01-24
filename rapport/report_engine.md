# Engine State Machine (Draft)

This document records the current engine state machine definitions, valid commands, and transition table.

## State Definitions

**Intro**
- Displays the game title and entry menu.
- Transitions: `Setup`, `PrintInstructions`, `Exit`.

**PrintInstructions**
- Scrollable rules view.
- Transition: back to `Intro`.

**Setup**
- Initializes map, decks, players, starting hands.
- Transitions: `PlayerAnnounce`.

**PlayerAnnounce**
- Announces the active player in the InfoPanel and sets TurnContext.
- Transitions: `PlayerTurn` for the active player, `WaitTurn` for inactive players.

**WaitTurn**
- Inactive player state (no input).
- Transition: when player becomes active → `PlayerAnnounce`.

**PlayerTurn**
- Main command hub for the active player.
- Transitions: `DestinationCard`, `FaceUp`, `FaceDown`, `ClaimRoad`, `ClaimStation`, `BorrowRoad`, `Confirmation`.

**DestinationCard**
- Draws 3 destination cards and stores them as pending.
- Transition: `DestinationChoice`.

**DestinationChoice**
- Player selects which cards to keep (min 1).
- Unkept cards return to the deck.
- Transition: `Confirmation`.

**FaceUp**
- Handles drawing a face-up wagon card.
- Locomotive drawn first ends draw phase immediately.
- Transition: `Confirmation`.

**FaceDown**
- Handles drawing a face-down wagon card.
- Transition: `Confirmation` after draw phase completes.

**ClaimRoad**
- Validates and begins road claiming.
- If tunnel → `TunnelResolve`, else → `RoadResolve`.
- On failure → `PlayerTurn`.

**TunnelResolve**
- Draws extra cards to resolve tunnel cost.
- If player can pay, road is acquired and cards removed.
- Transition: `Confirmation`.

**RoadResolve**
- Applies ownership, updates wagons and score.
- Transition: `Confirmation`.

**ClaimStation**
- Validates and applies station build.
- Transition: `Confirmation`.

**BorrowRoad**
- Validates and registers a borrowed road.
- Transition: `Confirmation`.

**Confirmation**
- Allows only: `EndTurn` or `BorrowRoad`.
- Transition: `EndTurn`.

**EndTurn**
- Advances active player and checks last-round logic.
- Transition: `PlayerAnnounce` or `EndGame`.

**EndGame**
- Final scoring, determine winner.
- Transition: `Exit`.

**Exit**
- Cleanup and quit.

## Commands

Commands are sent from the TUI and interpreted by the engine:

- `DRAW DEST` → DestinationCard
- `SELECT <number>` → DestinationChoice
- `DRAW CARD FACEUP <index>` → FaceUp
- `DRAW CARD FACEDOWN` → FaceDown
- `TAKE <stationA>-<stationB> [roadId]` → ClaimRoad
- `BUILD STATION <station>` → ClaimStation
- `BORROW ROAD <stationA>-<stationB> [roadId]` → BorrowRoad
- `END TURN` → Confirmation → EndTurn
- `EXIT` → Exit

*(Exact argument structure will match the JSON command schema used by the parser.)*

## Transition Table (Summary)

| From State | Trigger / Condition | To State |
|-----------|---------------------|---------|
| Intro | Start | Setup |
| Intro | Instructions | PrintInstructions |
| Intro | Exit | Exit |
| PrintInstructions | Back | Intro |
| Setup | Done | PlayerAnnounce |
| PlayerAnnounce | Active player | PlayerTurn |
| PlayerAnnounce | Inactive player | WaitTurn |
| WaitTurn | Player becomes active | PlayerAnnounce |
| PlayerTurn | DRAW DEST | DestinationCard |
| PlayerTurn | DRAW CARD FACEUP | FaceUp |
| PlayerTurn | DRAW CARD FACEDOWN | FaceDown |
| PlayerTurn | TAKE road | ClaimRoad |
| PlayerTurn | BUILD STATION | ClaimStation |
| PlayerTurn | BORROW ROAD | BorrowRoad |
| PlayerTurn | END TURN | Confirmation |
| DestinationCard | Draw complete | DestinationChoice |
| DestinationChoice | SELECT <number> | Confirmation |
| FaceUp | Draw complete | Confirmation |
| FaceDown | Draw complete | Confirmation |
| ClaimRoad | Tunnel | TunnelResolve |
| ClaimRoad | Non-tunnel | RoadResolve |
| ClaimRoad | Failure | PlayerTurn |
| TunnelResolve | Resolved | Confirmation |
| RoadResolve | Applied | Confirmation |
| ClaimStation | Applied | Confirmation |
| BorrowRoad | Applied | Confirmation |
| Confirmation | End turn | EndTurn |
| Confirmation | Borrow road | BorrowRoad |
| EndTurn | Next player | PlayerAnnounce |
| EndTurn | Last round complete | EndGame |
| EndGame | Done | Exit |

