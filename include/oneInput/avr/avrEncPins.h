#pragma once
#ifdef __AVR__
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

namespace oneInput::avr {

  // Enables PCINT for two encoder channel bits on any AVR port.
  // begin() seeds the Encoder above via initEncoder(ab) so _prevAB is correct from start.
  // dispatch() reads both bits atomically (one PORT read) and calls onEdge(ab).
  // Call dispatch() from the ISR in user code:
  //   ISR(PCINT1_vect) { MyEnc::dispatch(); }
  /// @brief AVR two-pin PCINT attachment for quadrature encoder; dispatch() calls onEdge(ab)
  template<uint8_t Group, typename Port, uint8_t BitA = 0, uint8_t BitB = 1>
  struct AvrEncPins {
    static constexpr uint8_t mask = uint8_t((1u << BitA) | (1u << BitB));

    template<typename O>
    struct Part : O {
      static uint8_t readAB() {
        uint8_t pin = *reinterpret_cast<volatile uint8_t*>(Port::pinAddr);
        return uint8_t(((pin >> BitA) & 1u) | (((pin >> BitB) & 1u) << 1u));
      }

      static void begin() {
        *reinterpret_cast<volatile uint8_t*>(Port::ddrAddr)  &= uint8_t(~mask);
        *reinterpret_cast<volatile uint8_t*>(Port::portAddr) |= mask;
        if constexpr (Group == 0) PCMSK0 |= mask;
        else if constexpr (Group == 1) PCMSK1 |= mask;
        else PCMSK2 |= mask;
        PCICR |= uint8_t(1u << Group);
        O::initEncoder(readAB());  // seed Encoder::_prevAB with actual pin state
        O::begin();
        sei();
      }

      static void dispatch() { O::onEdge(readAB()); }
    };
  };

} // oneInput::avr
#endif
