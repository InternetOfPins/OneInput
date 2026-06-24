#pragma once
#include <oneInput/inputDef.h>

namespace oneInput {

  // Top-of-hardware-chain event buffer for button chains.
  // Click and Hold bubble onClick()/onHold() up via O::onClick() — this struct
  // intercepts them and stores a pending flag for BtnIn<HW> to poll.
  //
  // Default commands:
  //   onClick → Cmd 1  (= Cmd::Enter in oneMenu; BtnIn maps these)
  //   onHold  → Cmd 2  (= Cmd::Esc  in oneMenu; BtnIn maps these)
  // BtnIn<HW, ClickCmd, HoldCmd> translates them to actual CKE.
  /// @brief button event buffer; captures onClick/onHold into pending flags for BtnIn<HW> to drain
  struct BtnCapture {
    template<typename O>
    struct Part : O {
      inline static volatile uint8_t _pending = 0;  // 0=none 1=click 2=hold

      static void onClick() { _pending = 1; }
      static void onHold()  { _pending = 2; }

      static bool    available() { O::available(); return _pending != 0; }  // triggers Hold::check
      static uint8_t pending()   { return _pending; }                          // peek without consume
      static uint8_t take()      { uint8_t r = _pending; _pending = 0; return r; }
    };
  };

} // oneInput
