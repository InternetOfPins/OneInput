#pragma once
#include <stdint.h>

namespace oneInput {

  /// @brief time-gated debounce; suppresses onRise/onFall transitions within Ms of each other
  template<uint16_t Ms = 20>
  struct Debounce {
    template<typename O>
    struct Part : O {
      inline static uint32_t _last = 0;

      static void onRise() {
        uint32_t now = millis();
        if (uint32_t(now - _last) >= Ms) { _last = now; O::onRise(); }
      }
      static void onFall() {
        uint32_t now = millis();
        if (uint32_t(now - _last) >= Ms) { _last = now; O::onFall(); }
      }
    };
  };

} // oneInput
