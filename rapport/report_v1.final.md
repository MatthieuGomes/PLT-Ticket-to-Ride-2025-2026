# State description

The game state is divided in 3 sections :
1. The map
2. The cards
3. The players

## 1. Map

The `mapState` is composed of :
1. A list of stations
2. A list of roads to connect the stations

### Stations

### Roads

There are 3 types of roads :
1. Normal roads
2. Ferry roads
3. Tunnel roads

#### Normal roads

#### Ferry roads

#### Tunnel roads

## 2. Cards

Their are 2 types of cards :
1. Wagon cards
2. Destination cards

### Wagon cards

### Destination cards

Their are 2 types of destination cards :
1. Normal destination cards
2. Long destination cards
but their implementation is the same.

The cards are gathered in `deck`. There are multiple significant piles :
1. Face down deck
2. Face up cards
3. Discard pile (trash)
in addition, there is a deck called `outOfGame` which contains destination cards that are not used in the current game.

those decks can be gathered in deckgroups for easier manipulation. 
the first represents the cards shared by all players (`sharedDecks`), the second represents the cards held by each player (`playerDecks`).

each sharedDeck contains a face down deck, a face up deck and a discard pile.

each playerDeck contains a deck of destination cards and a deck of wagon cards.

so the `cardsState` is composed of 2 `sharedDecks` (one for each type of card), a list of `playerCards` (one for each player) and an outOfGame deck for destination cards.

## 3. Players

the `playersState` contains a list of players.

Each player has the following attributes :
1. color
2. score
3. number of wagons remaining
4. ...

