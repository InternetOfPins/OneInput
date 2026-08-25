#pragma once
#include <stdint.h>
#include <hapi/hapi.h>

namespace oneInput {

  /// @brief silent InputAPI terminal; hardware and filter components override via Part<O>
  struct InputAPI {
    static void begin()              {}
    static void initEncoder(uint8_t) {}  // called by AvrEncPins to seed Encoder::_prevAB
    static void onRise()             {}
    static void onFall()             {}
    static void onClick()            {}
    static void onHold()             {}
    static void onEdge(uint8_t)      {}  // encoder: current AB pair (bits 0-1)
    [[nodiscard]] static bool available()          { return false; }
  };

  /// @brief composes a hardware/filter chain over InputAPI.
  template<typename... OO>
  using InputDef = hapi::APIOf<InputAPI, OO...>;

} // oneInput
