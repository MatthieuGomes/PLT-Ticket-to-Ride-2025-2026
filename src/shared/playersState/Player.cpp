
// ----- Setters -----

void Player::setId(int id) {
    this->id = id;
}

void Player::setName(string name) {
    this->name = name;
}

void Player::setNbStations(int nbStations) {
    this->nbStations = nbStations;
}

void Player::setNbRoads(int nbRoads) {
    this->nbRoads = nbRoads;
}


void Player::drawCard(string deck) {
    wagonDeck.push_back(deck);
    cout << name << " pioche une carte : " << deck << endl;
}