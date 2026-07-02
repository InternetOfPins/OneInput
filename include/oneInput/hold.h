#pragma once
#include <stdint.h>

namespace oneInput {

  // Detects a hold: button pressed continuously for HoldMs.
  // onHold() fires once (via available() poll) and suppresses the Click above it:
  //   Click::onRise() fires onClick() only if millis()-_t <= Click::MaxMs.
  //   Since HoldMs > MaxMs (by design), Click's window is already expired when Hold fires.
  // Place ABOVE Click in the chain:  InputDef<BtnIn, Hold<800>, Click<300>, ...>
  /// @brief hold detector: onFall held ≥ HoldMs → onHold() via available(); suppresses click naturally
  template<uint16_t HoldMs = 800>
  struct Hold {
    template<typename O>
    struct Part : O {
      inline static volatile uint32_t _t     = 0;
      inline static volatile bool     _dn    = false;
      inline static volatile bool     _fired = false;

      static void onFall() { _t = millis(); _dn = true; _fired = false; O::onFall(); }
      static void onRise() { _dn = false; O::onRise(); }

      static bool available() {
        if (_dn && !_fired && uint32_t(millis() - _t) >= HoldMs) {
          _fired = true;
          O::onHold();
        }
        return O::available();
      }
    };
  };

} // oneInput
