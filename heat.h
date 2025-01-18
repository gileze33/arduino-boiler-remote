// heat.h

#ifndef HEAT_DEFS_H
#define HEAT_DEFS_H

#include <Arduino.h> // Include Arduino types like uint8_t

// Define the Bit struct
struct Bit {
  float lengthInSamples;
  uint8_t value;
  char descriptor;
};

struct Sequence {
  int length;
  Bit* bits;
};

void demandHeat(bool on);

#endif