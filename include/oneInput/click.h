#pragma once
#include <stdint.h>

namespace oneInput {

  // Detects a click: fall followed by rise within MaxMs.
  // onClick() fires on the rising edge (release), not on press.
  // If the button is held longer than MaxMs, no click fires — Hold can take over.
  /// @brief click detector: onFall+onRise within MaxMs → onClick(); longer press → silent
  template<uint16_t MaxMs = 300>
  struct Click {
    template<typename O>
    struct Part : O {
      inline static volatile uint32_t _t  = 0;
      inline static volatile bool     _dn = false;

      static void onFall() { _t = millis(); _dn = true;  O::onFall(); }
      static void onRise() {
        if (_dn && uint32_t(millis() - _t) <= MaxMs) O::onClick();
        _dn = false;
        O::onRise();
      }
    };
  };

} // oneInput
