# OneInput

Composable physical input library for IOP — debounce, click, hold, and quadrature encoder, assembled as HAPI chains. Zero dynamic allocation, pure-static dispatch.

Part of the [InternetOfPins](https://github.com/InternetOfPins) project family.

## Overview

Input chains are assembled outermost (menu adapter) to innermost (ISR attachment). The same components work on AVR (PCINT), ESP32/ESP8266 (attachInterrupt), or any other target that provides the `onEdge` / `onRise` / `onFall` primitives.

The chain `begin()` wires all interrupts. After that, dispatching is driven by either a user ISR (AVR) or `attachInterrupt` callbacks (ESP32/ESP8266).

## Components

| Component | Role |
|---|---|
| `Encoder` | Quadrature decoder: `onEdge(ab)` → `onUp()` / `onDown()` |
| `BtnCapture` | ISR→queue bridge: `onRise()` / `onFall()` → polled event queue |
| `Debounce<Ms>` | Rejects transitions shorter than `Ms` milliseconds |
| `Click<Ms>` | Detects press+release within `Ms` ms → `onClick()` |
| `Hold<Ms>` | Detects press held beyond `Ms` ms → `onHold()` |

## Platform pin cores

### AVR — `PCINT`-driven (direct register)

```cpp
#include <oneInput/oneInput.h>
#include <chips/avr/avrPort.h>

// Encoder on PC0+PC1 (Arduino A0+A1), PCINT group 1
using EncHW = hapi::APIOf<oneInput::InputDef,
  oneInput::Encoder,
  oneInput::avr::AvrEncPins</*group*/1, hw::avr::chip::PortC, /*bitA*/0, /*bitB*/1>
>;

// Button on PC2 (Arduino A2)
using BtnHW = hapi::APIOf<oneInput::InputDef,
  oneInput::BtnCapture,
  oneInput::Hold<800>,
  oneInput::Click<300>,
  oneInput::Debounce<20>,
  oneInput::avr::AvrBtnPin</*group*/1, hw::avr::chip::PortC, /*bit*/2>
>;

// User must provide the ISR
ISR(PCINT1_vect) { EncHW::dispatch(); BtnHW::dispatch(); }
```

### ESP32 / ESP8266 — `attachInterrupt`-driven

```cpp
#include <oneInput/oneInput.h>

// No user ISR — begin() calls attachInterrupt internally
using EncHW = hapi::APIOf<oneInput::InputDef,
  oneInput::Encoder,
  oneInput::Esp32EncPins<32, 33>   // CLK=32, DT=33
>;
using BtnHW = hapi::APIOf<oneInput::InputDef,
  oneInput::BtnCapture,
  oneInput::Hold<800>,
  oneInput::Click<300>,
  oneInput::Debounce<20>,
  oneInput::Esp32BtnPin<25>
>;
```

## Wiring to OneMenu

`EncIn<HW, Steps>` and `BtnIn<HW>` are HAPI components that translate hardware events into menu navigation commands (`CKE::Up`, `CKE::Down`, `CKE::Enter`, `CKE::Esc`).

```cpp
#include <oneMenu/menu/IO/IOP/encIn.h>
#include <oneMenu/menu/IO/IOP/btnIn.h>

using Enc = oneMenu::EncIn<EncHW, 4>;  // 4 steps per detent
using Btn = oneMenu::BtnIn<BtnHW>;

// Use inside InDef (alongside PCKbd, SerialIn, etc.)
InDef<Enc, Btn, SerialIn, PCKbd> in;
```

`Steps` is the number of quadrature transitions per physical detent — 4 for most encoders, 2 for half-step types.

## Dependencies

- [HAPI](https://github.com/InternetOfPins/HAPI)
- [OneChip](https://github.com/InternetOfPins/OneChip) — for AVR port types (`AvrEncPins`, `AvrBtnPin`)

## License

MIT — see [LICENSE](LICENSE).

*Author: Rui Azevedo (neu-rah) · Azores, Portugal*
