# State description


## 0. State
The game state is divided in 3 sections :
1. **map** : the state of the map {shared ptr to [MapState](#map-state)}
2. **cards** : the state of the cards {shared ptr to [CardsState](#cards-state)}
3. **players** : the state of the players {shared ptr to [PlayersState](#players-state)}

## 1. Map

### Station

A station has the following attributes :
1. **name :** the name of the station {string}
2. **owner :** which player owns the station, null if unclaimed {shared ptr to [Player](#player)}
3. **vertex :** the graph representation of the station {boost graph vertex descriptor}

### Road

There are 3 types of roads :
1. [Normal road](#normal-road)
2. [Ferry road](#ferry-road)
3. [Tunnel road](#tunnel-road)


#### Normal road

Attributes of a normal road :
1. **id :** unique identifier of the road {int}
2. **length :** the length of the road {int}
3. **color :** the color of the road, grey if any color can be used {[RoadColor](#road-color)}
4. **stationA :** on of the two extremities of the road {shared ptr to[Station](#station)}
5. **stationB :** on of the two extremities of the road {shared ptr to [Station](#station)}
6. **owner :** which player owns the road, null if unclaimed {shared ptr to [Player](#player)}
7. **edge :** the graph representation of the road {boost graph edge descriptor}

there are additional static attributes such as : 
1. **possibleColors :** list of possible colors for the road {[RoadColor\[10\]](#road-color)}
2. **ColorsNames :** mapping of RoadColor to string {unordered_map of [RoadColor](#road-color) to string}

#### Ferry road

These have the same attributes as normal roads with an additional one :
1. **locomotives :** number of locomotive cards required to claim the road {int}

#### Tunnel road

These are identical to normal roads but have other mechanics during claiming.

### Road color

A colorCard represents the color of a wagon card. It can be one of the following values :
1. Red
2. Blue
3. Green
4. Yellow
5. Black
6. White
7. Orange
8. Pink
9. Any (wildcard)

A tenth value `Unknown` is used when a color is not yet assigned  for developpment purposes.

### Map State
The `mapState` attributes contains :
1. **stations :** A list of [Station](#station) in the game {vector of shared ptr to [Station](#station)}
2. **roads :** A list of [Road](#road) to connect the stations {{vector of shared ptr to [Road](#road)}}
3. **gameGraph :** the boost graph representation of the map {boost adjacency list}


## 2. Cards

Their are 2 types of cards :
1. Wagon cards [see wagon cards](#wagon-cards)
2. Destination cards [see destination cards](#destination-cards)

### Wagon cards

Attributes of a wagon card :
1. **color :** the color of the wagon card {[ColorCard](#color-card)}

There are static attributes such as :
1. **locomotiveInitCardsCount :** number of locomotive cards in the initial deck of a new game {int}
2. **colorInitCardsCount :** number of cards of each color in the initial deck of a new game {int}
3. **faceUpCardsCount :** number of face up cards visible to players at the start of the game {int}
4. **possibleColors :** list of possible colors for the wagon cards {[ColorCard\[10\]](#color-card)}
5. **ColorsNames :** mapping of ColorCard to string {unordered_map of [ColorCard](#color-card) to string}


### Destination cards

Their are 2 types of destination cards :
1. Normal destination cards
2. Long destination cards
but their implementation is the same.

Attributes of a destination card :
1. **stationA :** one extremity of the destination {shared ptr to [Station](#station)}
2. **stationB :** the other extremity of the destination {shared ptr to [Station](#station)}
3. **points :** the number of points the destination is worth {int}
4. **isLong :** whether the destination is a long destination or not {bool}

### Cards organization

#### Decks
The cards are gathered in `deck`. There are multiple significant piles :
1. Face down deck
2. Face up cards
3. Discard pile (trash)
in addition, there is a deck called `outOfGame` which contains destination cards that are not used in the current game.

#### Deckgroups

those decks can be gathered in deckgroups for easier manipulation. 
the first represents the cards shared by all players (`sharedDeck`), the second represents the cards held by each player (`playerDeck`).
##### SharedDeck
Each sharedDeck contains a face down deck, a face up deck and a discard pile.

##### PlayerDeck
Each playerDeck contains a deck of destination cards and a deck of wagon cards.

### Color card

A colorCard represents the color of a wagon card. It can be one of the following values :
1. Red
2. Blue
3. Green
4. Yellow
5. Black
6. White
7. Orange
8. Pink
9. Locomotive (wildcard)

A tenth value `Unknown` is used when a color is not yet assigned  for developpment purposes.

### Cards State

The `CardsState` attributes contains all the decks and deckgroups used in the game :

1. **Game wagon cards :** a sharedDeck containing the face down deck, face up cards and discard pile of wagon cards {shared ptr to [SharedDeck\<WagonCards\>](#shareddeck)}  
2. **Game destination cards :** a sharedDeck containing the face down deck, face up cards and discard pile of destination cards {shared ptr to [SharedDeck\<DestinationCards\>](#shareddeck)}
3. **playerCards :** a list of [PlayerDeck](#playerdeck) (one for each player) {vector of shared ptr to [PlayerDeck](#playerdeck)}
4. **outOfGame :** a deck containing destination cards that are not used in the current game {shared ptr to [OutOfGame\<DestinationCards\>](#deck)}

## 3. Players

### Players State
the `playersState` contains a list of [Player](#player).

### Player
Each player has the following attributes :
1. **name** : the name of the player {string}
2. **color** : the color representing the player {[PlayerColor](#player-color)}
3. **score** : the current score of the player {int}
4. **nbWagon** : the number of wagons remaining for the player {int}
5. **nbStation** : the number of station tokens remaining for the player {int}
6. **hand** : cards of the player [see PlayerDeck](#playerdeck)
7. **borrowedRoads** : list of roads borrowed by the player (linked to station owning mechanic)  {vector of shared ptr to [Road](#road)} 
8. **completedDestinations** : a list of completed destination cards {vector of shared ptr to [DestinationCard](#destination-cards)}

There are additional static attributes such as :
1. **initialWagonsCount :** number of wagons each player starts with at the beginning of the game {int}
2. **initialStationsCount :** number of station tokens each player starts with at the beginning of the game {int}
3. **startScore :** initial score of each player at the beginning of the game {int} 
4. **possibleColors :** list of possible colors for the players {[PlayerColor\[6\]](#player-color)}
5. **ColorsNames :** mapping of PlayerColor to string {unordered_map of [PlayerColor](#player-color) to string}
6. **PointsByRoadLength :** mapping of road length to points awarded when claiming the road {unordered_map of int to int}

### Player color

A playerColor represents the color of a player. It can be one of the following values :
1. Red
2. Blue
3. Green
4. Yellow
5. Black

a 6th color `Unknown` is used when a color is not yet assigned for developpment purposes.

