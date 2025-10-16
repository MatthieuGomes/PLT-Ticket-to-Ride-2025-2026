#pragma once
#include <cstdint>

class LcgGenerator {
private:
    uint16_t mCurrentX, mM, mA, mC;

public:
    LcgGenerator(uint16_t a, uint16_t c, uint16_t x0, uint16_t m);
    uint16_t next();
};
