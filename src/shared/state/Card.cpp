#include "Card.h"

Card::Card(int id_) : id(id_), state(nullptr) {}
Card::~Card() = default;

int Card::getId() const { return id; }
void Card::setState(CardState* s) { state = s; }
