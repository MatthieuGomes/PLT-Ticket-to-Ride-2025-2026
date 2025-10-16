#include "LcgGenerator.hpp"

LcgGenerator::LcgGenerator(uint16_t a, uint16_t c, uint16_t x0, uint16_t m)
    : mA(a), mC(c), mCurrentX(x0), mM(m) {}

uint16_t LcgGenerator::next() {
    mCurrentX = (mA * mCurrentX + mC) % mM;
    return mCurrentX;
}
