#pragma once
#if defined(ESP32) || defined(ESP8266)
#include <Arduino.h>

namespace oneInput::esp32 {

  // attachInterrupt-based button pin for ESP32/ESP8266.
  // No port/group concept — each GPIO has its own interrupt.
  // dispatch() is registered directly via attachInterrupt in begin().
  //
  // N: GPIO number (any interrupt-capable pin).
  // Active-low: pullup enabled; onFall = pressed, onRise = released.
  //
  // No user ISR required — begin() calls attachInterrupt internally.
  /// @brief ESP32/8266 single-pin interrupt button; attachInterrupt on CHANGE → onRise/onFall
  template<int N>
  struct Esp32BtnPin {
    template<typename O>
    struct Part : O {
      inline static uint8_t _prev = 1;

      static void begin() {
        pinMode(N, INPUT_PULLUP);
        _prev = digitalRead(N) ? 1u : 0u;
        attachInterrupt(digitalPinToInterrupt(N), dispatch, CHANGE);
        O::begin();
      }

      static void IRAM_ATTR dispatch() {
        uint8_t curr = digitalRead(N) ? 1u : 0u;
        if (curr == _prev) return;
        _prev = curr;
        if (curr) O::onRise();
        else       O::onFall();
      }
    };
  };

} // oneInput::esp32

namespace oneInput { using namespace oneInput::esp32; }
#endif
