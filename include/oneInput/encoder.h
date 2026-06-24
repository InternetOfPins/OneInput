#pragma once
#include <stdint.h>

namespace oneInput {

  // Quadrature decoder. Sits above AvrEncPins in the chain.
  // AvrEncPins calls onEdge(ab) with the current 2-bit AB state on each ISR.
  // AvrEncPins also calls initEncoder(ab) from begin() to seed _prevAB correctly.
  //
  // delta() is the accumulated tick count (positive = CW, negative = CCW).
  // consume(n) removes n ticks — EncIn calls this after producing a CKE event.
  /// @brief quadrature decoder; onEdge(ab) updates delta; consume(n) removes ticks after dispatch
  struct Encoder {
    static constexpr int8_t _dec[16] = {
       0, -1,  1,  0,
       1,  0,  0, -1,
      -1,  0,  0,  1,
       0,  1, -1,  0
    };

    template<typename O>
    struct Part : O {
      inline static volatile int8_t _delta  = 0;
      inline static          uint8_t _prevAB = 0;

      static void initEncoder(uint8_t ab) { _prevAB = ab; O::initEncoder(ab); }

      static void onEdge(uint8_t currAB) {
        _delta += _dec[(_prevAB << 2) | currAB];
        _prevAB = currAB;
        O::onEdge(currAB);
      }

      static int8_t delta()         { return _delta; }
      static void   consume(int8_t n) { _delta -= n; }

      static bool available() { return O::available(); }
    };
  };

} // oneInput
