#pragma once
#ifdef __AVR__
#include <stdint.h>

namespace oneInput::avr {

  // Two-channel analog-joystick channel selector. Sits directly above
  // hw::avr::AVRAdc<Div> (OneChip) in the InputDef chain and exposes readX()/readY()
  // by calling its read(channel) — the terminal oneInput::Joystick<> polls.
  //
  //   using JoyHW = oneInput::InputDef<
  //     oneInput::Joystick<512,96,200>,
  //     oneInput::avr::AvrAdcAxes<0,1>,   // X=A0, Y=A1
  //     hw::avr::AVRAdc<>
  //   >;
  /// @brief exposes readX()/readY() over hw::avr::AVRAdc's read(channel), one channel per axis
  template<uint8_t ChX, uint8_t ChY>
  struct AvrAdcAxes {
    template<typename O>
    struct Part : O {
      using Base=O;
      static uint16_t readX() { return Base::read(ChX); }
      static uint16_t readY() { return Base::read(ChY); }
    };
  };

} // oneInput::avr
#endif // __AVR__
