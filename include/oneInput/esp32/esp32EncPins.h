#pragma once
#if defined(ESP32) || defined(ESP8266)
#include <Arduino.h>

namespace oneInput::esp32 {

  // attachInterrupt-based encoder pins for ESP32/ESP8266.
  // Both PinA and PinB attach the same dispatch() — any change on either triggers
  // a re-read of both pins atomically via two consecutive digitalRead calls.
  // (Two-read races are corrected naturally by the quadrature decoder on next ISR.)
  //
  // begin() registers both interrupts; no user ISR required.
  /// @brief ESP32/8266 two-pin interrupt encoder; CHANGE on either pin → onEdge(ab)
  template<int PinA, int PinB>
  struct Esp32EncPins {
    template<typename O>
    struct Part : O {
      static uint8_t IRAM_ATTR readAB() {
        return uint8_t(uint8_t(digitalRead(PinA) ? 1u : 0u) |
                       uint8_t(digitalRead(PinB) ? 2u : 0u));
      }

      static void begin() {
        pinMode(PinA, INPUT_PULLUP);
        pinMode(PinB, INPUT_PULLUP);
        O::initEncoder(readAB());
        attachInterrupt(digitalPinToInterrupt(PinA), dispatch, CHANGE);
        attachInterrupt(digitalPinToInterrupt(PinB), dispatch, CHANGE);
        O::begin();
      }

      static void IRAM_ATTR dispatch() { O::onEdge(readAB()); }
    };
  };

} // oneInput::esp32

namespace oneInput { using namespace oneInput::esp32; }
#endif
