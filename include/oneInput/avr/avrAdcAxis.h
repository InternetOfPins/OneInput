#pragma once
#ifdef __AVR__
#include <stdint.h>

namespace oneInput::avr {

  // One-channel analog-axis channel selector — the 1-axis sibling of
  // AvrAdcAxes<ChX,ChY> (avrAdcAxes.h). Sits directly above
  // hw::avr::AVRAdc<Div> (OneChip) in the InputDef chain and exposes read()
  // by calling its read(channel) — the terminal oneInput::AnalogAxis<>
  // polls this the same way AM4's own real analogAxis<pin,...> calls
  // analogRead(pin) directly, except pin/channel selection is its own
  // composable layer here instead of baked into the filter.
  //
  //   using AxisHW = oneInput::InputDef<
  //     oneInput::AnalogAxis<512,6,10,1>,
  //     oneInput::avr::AvrAdcAxis<0>,   // A0
  //     hw::avr::AVRAdc<>
  //   >;
  /// @brief exposes read() over hw::avr::AVRAdc's read(channel), one fixed channel
  template<uint8_t Ch>
  struct AvrAdcAxis {
    template<typename O>
    struct Part : O {
      using Base = O;
      static uint16_t read() { return Base::read(Ch); }
    };
  };

} // oneInput::avr
#endif // __AVR__
