#pragma once
#include <stdint.h>

namespace oneInput {

  // Single-axis analog joystick filter: bit-shift deadzone + inverse-scaled
  // key-repeat, sitting above a raw 1-axis ADC reader (O::read(), a raw
  // sample — e.g. 0..1023 on AVR's 10-bit ADC). Matches AM4's
  // real analogAxis<pin,sensivity,inv,center,inertia,bufferSz,posCmd,negCmd>
  // arithmetic exactly (checked against AM4's real analogAxisIn.h getStep()),
  // minus the parts that belong to other layers in a native OneInput chain:
  //   - `pin` is the ADC-channel-selector's job (oneInput::avr::AvrAdcAxis<Ch>
  //     below this in the chain), not the filter's — same split
  //     Joystick/AvrAdcAxes already use for the 2-axis case.
  //   - `inv` (mount/wiring sign flip) is InvDelta<W,Inv>'s job (below this
  //     component), matching oneData::InvDir/oneInput::InvAxis's own "wrap
  //     the already-filtered output" placement rather than a filter param.
  //   - `posCmd`/`negCmd` are oneMenu::AxisIn<HW,PosCmd,NegCmd>'s own
  //     template params (the OneMenu-facing adapter), matching JoyIn's
  //     Cmd-pairing precedent — this filter only ever emits a signed tick.
  //
  //   Center:    raw ADC value at rest (AM4: center).
  //   Inertia:   deadzone via bit-shift, NOT a linear threshold — a=|read-
  //              Center|; if(!(a>>Inertia)) return; — AM4's exact "must-
  //              exceed" gate, not a plain a>threshold compare.
  //   Sensivity: repeat-tick interval scales inversely with deflection:
  //              ms_between_ticks ~= 1e6/a/(Sensivity+1) — AM4's exact
  //              formula, computed in integer arithmetic here rather than
  //              AM4's float (same no-float-on-embedded convention
  //              Joystick's own integer-only arithmetic already follows —
  //              truncates towards zero instead of AM4's float rounding, no
  //              practical difference for a repeat-rate heuristic).
  //   BufferSz:  caps how many un-consumed ticks can accumulate in either
  //              direction (AM4: abs(cnt)<=bufferSz gate before adding).
  //
  // delta()/consume(n) mirror Encoder::delta()/consume(n) and Joystick's own
  // dx()/consumeX(n) — the idiomatic OneInput shape for "a filtered,
  // one-tick-per-call signed delta" — rather than AM4's own Stream-shaped
  // peek()/read() (tied to menuIn's Stream base, not needed for a native
  // chain — nothing here consumes that shape).
  //
  // setFieldMode(bool)/fieldMode() are kept, same names AM4 uses, since
  // oneMenu::AxisIn (menu/IO/IOP/axisIn.h) reads this flag to reproduce
  // AM4's own posCmd/negCmd XOR-on-field_mode behavior (AM4: direction
  // flips while editing a field). KNOWN GAP: nothing in OneMenu's InDef/Nav
  // machinery automatically calls setFieldMode() on entering/leaving edit
  // mode the way AM4's navRoot does on every registered menuIn (InAPI stays
  // deliberately Nav-agnostic — no such broadcast hook exists, and adding
  // one would be a genuine OneMenu core change, out of scope here). A sketch
  // that wants the flip wires it manually from a FIELD()'s own enter/exit
  // EventCall (e.g. `axisFieldToggle` calling `AxisHW::setFieldMode(true/
  // false)` directly) — opt-in, sketch-side, same "opt-in wiring, not
  // automatic" precedent already established by am4compat::IdleTimeout.
  /// @brief single-axis analog filter: bit-shift deadzone + inverse-scaled repeat, AM4-exact arithmetic
  template<int16_t Center = 512, uint8_t Inertia = 6, uint8_t Sensivity = 5, int8_t BufferSz = 1>
  struct AnalogAxis {
    template<typename O>
    struct Part : O {
      inline static volatile int8_t _cnt = 0;
      inline static uint32_t _last = 0;
      inline static bool _fieldMode = false;

      static void setFieldMode(bool m) { _fieldMode = m; }
      [[nodiscard]] static bool fieldMode() { return _fieldMode; }

      static void poll() {
        uint16_t p = O::read();
        int16_t  a = int16_t(p) - Center;
        a = a < 0 ? int16_t(-a) : a;
        if (!(uint16_t(a) >> Inertia)) return;
        uint32_t s   = 1000000UL / uint32_t(a) / uint32_t(Sensivity + 1);
        uint32_t now = millis();
        uint32_t dt  = now - _last;
        int8_t absCnt = _cnt < 0 ? int8_t(-_cnt) : _cnt;
        if (absCnt <= BufferSz && dt > s) {
          _cnt = int8_t(_cnt + (p > uint16_t(Center) ? 1 : -1));
          _last = now;
        }
      }

      [[nodiscard]] static bool available() { poll(); return _cnt != 0 || O::available(); }
      [[nodiscard]] static int8_t delta() { return _cnt; }
      static void consume(int8_t n) { _cnt = int8_t(_cnt - n); }
    };
  };

  // Wraps an AnalogAxis-shaped W's already-filtered delta()/consume(), sign-
  // flipping it — same "wrap the semantic output, not the raw ADC" placement
  // as oneData::InvDir/oneInput::InvAxis. AM4's own `inv` XORs with
  // field_mode at read time; composing this wrapper below AxisIn achieves
  // the identical combined effect (mount-wiring flip here, field_mode flip
  // in AxisIn — two independent sign flips compose the same as AM4's one
  // combined XOR).
  /// @brief inverts an AnalogAxis-shaped W's delta()/consume() sign (mount/wiring correction)
  template<typename W, bool Inv = false>
  struct InvDelta {
    template<typename O>
    struct Part : W::template Part<O> {
      using Base = typename W::template Part<O>;
      using Base::Base;
      [[nodiscard]] static int8_t delta() { int8_t v = Base::delta(); return Inv ? int8_t(-v) : v; }
      static void consume(int8_t n) { Base::consume(Inv ? int8_t(-n) : n); }
    };
  };

} // oneInput
