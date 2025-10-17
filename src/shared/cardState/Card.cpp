#include "Card.h"

cardState::Card::Card(int id_) : id(id_), state(nullptr) {}

cardState::Card::~Card() = default;

int cardState::Card::getId() { return id; }
void cardState::Card::setState(CardState* s) { state = s; }
