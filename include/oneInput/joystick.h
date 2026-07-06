#pragma once
#include <stdint.h>

namespace oneInput {

  // Analog joystick axis filter: deadzone + key-repeat, sitting above a raw 2-axis ADC
  // reader (O::readX()/O::readY(), each a raw sample — e.g. 0..1023 on AVR's 10-bit ADC).
  // Center/Deadzone/RepeatMs are in raw ADC units / milliseconds.
  //
  //   Center:   raw value read when the stick is physically centered (datasheet: ~half
  //             of full scale, e.g. 512 for a 0..1023 10-bit read at Vcc/2).
  //   Deadzone: +/- raw units around Center treated as "centered" (no event) — absorbs
  //             resting noise and mechanical center-return slop.
  //   RepeatMs: once past Deadzone, dx()/dy() ticks immediately (edge-triggered on
  //             entering/reversing direction, like a keypress), then again every RepeatMs
  //             while held in the same direction (like keyboard key-repeat) — available()
  //             polls/gates this the same way oneInput::Hold<HoldMs>::available() does.
  //
  // dx()/dy() return -1/0/+1 (one repeat tick, not a magnitude) — JoyIn (OneMenu) maps
  // them to CKE Left/Right/Up/Down exactly like Encoder::delta() maps to Up/Down in EncIn.
  // consumeX(n)/consumeY(n) mirror Encoder::consume(n).
  /// @brief analog joystick axis filter: deadzone + key-repeat over a raw 2-axis ADC read
  template<int16_t Center=512, int16_t Deadzone=96, uint16_t RepeatMs=200>
  struct Joystick {
    template<typename O>
    struct Part : O {
      inline static volatile int8_t _dx = 0, _dy = 0;
      inline static uint32_t _tx = 0, _ty = 0;
      inline static int8_t _dirX = 0, _dirY = 0;  // last-seen off-center direction, -1/0/+1

      static void poll() {
        uint32_t now = millis();

        int16_t x = int16_t(O::readX()) - Center;
        int8_t  dirX = x > Deadzone ? 1 : x < -Deadzone ? int8_t(-1) : int8_t(0);
        if (dirX != 0 && (dirX != _dirX || uint32_t(now - _tx) >= RepeatMs)) { _dx = dirX; _tx = now; }
        _dirX = dirX;

        int16_t y = int16_t(O::readY()) - Center;
        int8_t  dirY = y > Deadzone ? 1 : y < -Deadzone ? int8_t(-1) : int8_t(0);
        if (dirY != 0 && (dirY != _dirY || uint32_t(now - _ty) >= RepeatMs)) { _dy = dirY; _ty = now; }
        _dirY = dirY;
      }

      static bool available() { poll(); return _dx != 0 || _dy != 0 || O::available(); }
      static int8_t dx() { return _dx; }
      static int8_t dy() { return _dy; }
      static void consumeX(int8_t n) { _dx -= n; }
      static void consumeY(int8_t n) { _dy -= n; }
    };
  };

} // oneInput
