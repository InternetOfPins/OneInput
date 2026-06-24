#pragma once
#ifdef __AVR__
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

namespace oneInput::avr {

  // Enables PCINT for one bit on any AVR port (Groups 0-2 = PORTB/C/D on ATmega328P).
  // dispatch() reads the pin and calls onRise()/onFall() on the chain above.
  // Call dispatch() from the ISR in user code:
  //   ISR(PCINT1_vect) { MyBtn::dispatch(); }
  /// @brief AVR single-pin PCINT attachment; dispatch() detects edge and calls onRise/onFall
  template<uint8_t Group, typename Port, uint8_t Bit>
  struct AvrBtnPin {
    static constexpr uint8_t mask = uint8_t(1u << Bit);

    template<typename O>
    struct Part : O {
      inline static uint8_t _prev = 1;  // high = not pressed (active-low + pullup)

      static void begin() {
        *reinterpret_cast<volatile uint8_t*>(Port::ddrAddr)  &= uint8_t(~mask);  // input
        *reinterpret_cast<volatile uint8_t*>(Port::portAddr) |= mask;             // pullup on
        if constexpr (Group == 0) PCMSK0 |= mask;
        else if constexpr (Group == 1) PCMSK1 |= mask;
        else PCMSK2 |= mask;
        PCICR |= uint8_t(1u << Group);
        _prev = (*reinterpret_cast<volatile uint8_t*>(Port::pinAddr) & mask) ? 1u : 0u;
        O::begin();
        sei();
      }

      static void dispatch() {
        uint8_t curr = (*reinterpret_cast<volatile uint8_t*>(Port::pinAddr) & mask) ? 1u : 0u;
        if (curr == _prev) return;
        _prev = curr;
        if (curr) O::onRise();
        else       O::onFall();
      }
    };
  };

} // oneInput::avr
#endif
