#pragma once
#include <stdint.h>

namespace oneInput {

  /// @brief silent InputDef terminal; hardware and filter components override via Part<O>
  struct InputDef {
    static void begin()              {}
    static void initEncoder(uint8_t) {}  // called by AvrEncPins to seed Encoder::_prevAB
    static void onRise()             {}
    static void onFall()             {}
    static void onClick()            {}
    static void onHold()             {}
    static void onEdge(uint8_t)      {}  // encoder: current AB pair (bits 0-1)
    static bool available()          { return false; }
  };

} // oneInput
