#pragma once
#include <oneInput/inputDef.h>

namespace oneInput {

  // Immediate-press event buffer — unlike BtnCapture (which needs Click/
  // Hold above it to interpret press DURATION into onClick()/onHold()),
  // this fires the instant the button goes down (onFall — active-low +
  // pullup convention every AvrBtnPin already uses) — no duration
  // ambiguity, no waiting for release. For a dedicated nav button wired
  // to one fixed command (e.g. a real 4-button Up/Down/Enter/Esc panel,
  // one pin per command, not a single multi-purpose button distinguishing
  // click vs. hold).
  /// @brief immediate-press event buffer (no click/hold duration distinction) — for one-pin-one-fixed-command buttons
  struct PressCapture {
    template<typename O>
    struct Part : O {
      inline static volatile bool _pending = false;

      static void onFall() { _pending = true; }

      [[nodiscard]] static bool available() { return _pending; }
      [[nodiscard]] static bool take() { bool r = _pending; _pending = false; return r; }
    };
  };

} // oneInput
