# OneInput API Reference

Input drivers & filtering layer. Composable hardware abstraction for buttons, encoders, joysticks.

## Quick Start

```cpp
#include <oneInput/oneInput.h>
#include <chips/avr/avrDevice.h>

using namespace oneInput;
using namespace hw::avr;

// Encoder: A=PC0, B=PC1, Button=PC2
using MyEnc = InputDef<
  Encoder,
  avr::AvrEncPins<1, chip::PortC, 0, 1>
>;

MyEnc::begin();
int8_t delta = MyEnc::delta();    // Accumulated encoder steps
if (delta) { handle_encoder(delta); }
```

---

## Core Components

### Encoders (Quadrature Decoders)

| Return | Function | Params | Description |
|--------|----------|--------|-------------|
| `int8_t` | `Encoder::delta()` | – | Accumulated step count since boot |
| `void` | `Encoder::reset()` | – | Clear accumulated delta |
| `void` | `Encoder::begin()` | – | Initialize encoder (called by InputDef) |

**Usage**:
```cpp
using Enc = InputDef<Encoder, avr::AvrEncPins<1, chip::PortC, 0, 1>>;
Enc::begin();
int8_t steps = Enc::delta();
```

### Buttons & Edge Detection

| Return | Function | Params | Description |
|--------|----------|--------|-------------|
| `bool` | `BtnCapture::pending()` | – | Button event ready? |
| `uint8_t` | `BtnCapture::flags()` | – | Current button flags (onClick, onHold) |
| `void` | `Click<Ms>::check(bool pressed)` | ms | Update click timeout (called by filter chain) |
| `bool` | `Click<Ms>::detected()` | – | Was a click completed? |
| `void` | `Hold<Ms>::check(bool pressed)` | ms | Update hold timeout |
| `bool` | `Hold<Ms>::detected()` | – | Was a hold timeout reached? |

**Usage**:
```cpp
using Btn = InputDef<
  BtnCapture,
  Debounce<20>,
  Click<300>,
  avr::AvrBtnPin<1, chip::PortC, 2>
>;

Btn::begin();
if (Btn::pending()) {
  uint8_t flags = Btn::flags();
  if (flags & Click<300>::onClick) { handle_click(); }
}
```

### Debouncing

| Return | Function | Params | Description |
|--------|----------|--------|-------------|
| `void` | `Debounce<Ms>::check()` | – | Poll debounce timer |
| `bool` | `Debounce<Ms>::stable()` | – | State stable for full debounce period? |

**Parameters**: `Debounce<20>` = 20ms stable window

### Joysticks & Analog Input

| Return | Function | Params | Description |
|--------|----------|--------|-------------|
| `int8_t` | `Joystick<Center, Deadzone, Repeat>::x()` | – | X-axis value (-127 to +127) |
| `int8_t` | `Joystick<Center, Deadzone, Repeat>::y()` | – | Y-axis value (-127 to +127) |
| `bool` | `Joystick<Center, Deadzone, Repeat>::changed()` | – | Axis moved outside deadzone? |

**Parameters**:
- `Center=512` — ADC value at center
- `Deadzone=96` — Ignore small movements
- `Repeat=200` — Repeat timer in ms

---

## Hardware Pin Drivers

### AVR

**Encoder Pins**:
| Return | Function | Params | Description |
|--------|----------|--------|-------------|
| `void` | `AvrEncPins<TC, Port, A, B>::begin()` | – | Configure encoder A/B pins, start timer |

**Button Pins**:
| Return | Function | Params | Description |
|--------|----------|--------|-------------|
| `void` | `AvrBtnPin<TC, Port, Pin>::begin()` | – | Configure button pin, enable interrupt |

**ADC Axes**:
| Return | Function | Params | Description |
|--------|----------|--------|-------------|
| `uint16_t` | `AvrAdcAxes<X, Y>::readX()` | – | Read X-axis ADC channel |
| `uint16_t` | `AvrAdcAxes<X, Y>::readY()` | – | Read Y-axis ADC channel |

### ESP32

**Encoder Pins**:
```cpp
using Enc = InputDef<
  Encoder,
  esp32::Esp32EncPins<34, 35>  // CLK=GPIO34, DT=GPIO35
>;
```

**Button Pins**:
```cpp
using Btn = InputDef<
  BtnCapture,
  Debounce<20>,
  Click<300>,
  esp32::Esp32BtnPin<32>  // GPIO32
>;
```

---

## Filter Chains

Compose filters for flexible input processing:

```cpp
using FilteredBtn = InputDef<
  BtnCapture,           // Capture raw state
  Debounce<20>,         // 20ms stable window
  Click<300>,           // Detect 300ms click
  Hold<1000>,           // Detect 1s hold
  avr::AvrBtnPin<...>   // Hardware driver
>;
```

**Execution order** (innermost to outermost):
1. Hardware driver reads GPIO
2. Debounce filters noise
3. Click/Hold timers detect events
4. BtnCapture reports flags

---

## InputDef Composition

Assemble a complete input device:

```cpp
template<typename... OO>
using InputDef = hapi::APIOf<InputAPI, OO...>;
```

**Example: Multi-input board**
```cpp
using Encoder = InputDef<
  oneInput::Encoder,
  avr::AvrEncPins<1, chip::PortC, 0, 1>
>;

using Button = InputDef<
  BtnCapture,
  Debounce<20>,
  Click<300>,
  Hold<800>,
  avr::AvrBtnPin<1, chip::PortC, 2>
>;

Encoder::begin();
Button::begin();

// Poll both
int8_t enc_delta = Encoder::delta();
uint8_t btn_flags = Button::flags();
```

---

## Integration with OneMenu

OneInput devices feed into OneMenu's input adapters:

```cpp
#include <oneMenu/encoderIn.h>

// Wrap OneInput encoder in OneMenu input adapter
using MenuEnc = oneMenu::EncIn<Encoder>;

MenuEnc enc;
if (enc.available()) {
  CKE cmd = enc.cmd();  // Returns Up, Down, or None
}
```

---

## Common Patterns

### Debounced Push Button
```cpp
using Btn = InputDef<
  BtnCapture,
  Debounce<20>,
  avr::AvrBtnPin<1, chip::PortC, 2>
>;

Btn::begin();
while(true) {
  if (Btn::pending()) {
    handle_button_press();
  }
}
```

### Rotary Encoder with Click
```cpp
using Enc = InputDef<
  Encoder,
  avr::AvrEncPins<1, chip::PortC, 0, 1>
>;

using EncBtn = InputDef<
  BtnCapture,
  Click<300>,
  avr::AvrBtnPin<1, chip::PortC, 2>
>;

Enc::begin();
EncBtn::begin();

int8_t delta = Enc::delta();
if (EncBtn::flags() & Click<300>::onClick) {
  handle_click();
}
```

### Analog Joystick with Deadzone
```cpp
using Joy = InputDef<
  Joystick<512, 96, 200>,  // Center, deadzone, repeat
  avr::AvrAdcAxes<0, 1>     // X=A0, Y=A1
>;

Joy::begin();
int8_t x = Joy::x();
int8_t y = Joy::y();
```

---

## See Also

- [OneChip](../../../OneChip/docs/REFERENCE.md) — Hardware platforms & interrupts
- [OnePin](../../../OnePin/docs/REFERENCE.md) — GPIO pin abstraction
- [OneMenu](../../../OneMenu/docs/REFERENCE.md) — Menu UI integration
