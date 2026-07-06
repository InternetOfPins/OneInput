#pragma once

/**
 * @file oneInput.h
 * @brief OneInput — composable physical input library for IOP.
 *
 * Components assemble via oneInput::InputDef<...> (= hapi::APIOf<InputAPI,...>) — outermost
 * (menu adapter) to innermost (ISR attachment):
 *
 *   // Encoder on PC0+PC1, 4 steps/detent → CKE Up/Down
 *   using Enc = oneInput::InputDef<
 *     oneMenu::EncIn<4>,
 *     oneInput::Encoder,
 *     oneInput::avr::AvrEncPins<1, chip::PortC, 0, 1>
 *   >;
 *
 *   // Button on PC2, click→Enter, hold→Esc
 *   using Btn = oneInput::InputDef<
 *     oneMenu::BtnIn<>,
 *     oneInput::Hold<800>,
 *     oneInput::Click<300>,
 *     oneInput::Debounce<20>,
 *     oneInput::avr::AvrBtnPin<1, chip::PortC, 2>
 *   >;
 *
 *   ISR(PCINT1_vect) { Enc::dispatch(); Btn::dispatch(); }
 *
 * ESP32/ESP8266 — no user ISR; begin() calls attachInterrupt internally:
 *
 *   using EncHW = oneInput::InputDef<
 *     oneInput::Encoder,
 *     oneInput::Esp32EncPins<34, 35>   // CLK=34, DT=35
 *   >;
 *   using BtnHW = oneInput::InputDef<
 *     oneInput::BtnCapture,
 *     oneInput::Hold<800>,
 *     oneInput::Click<300>,
 *     oneInput::Debounce<20>,
 *     oneInput::Esp32BtnPin<32>
 *   >;
 *   // No ISR needed — begin() wires attachInterrupt
 *   InDef<oneMenu::EncIn<EncHW, 4>, oneMenu::BtnIn<BtnHW>, PCKbd> in;
 *
 * Analog joystick (AVR only so far — needs hw::avr::AVRAdc, see OneChip/chips/avr/avrAdc.h):
 * no ISR, polled each Joystick::available() call like any plain analog read.
 *
 *   using JoyHW = oneInput::InputDef<
 *     oneInput::Joystick<512, 96, 200>,  // Center, Deadzone, RepeatMs
 *     oneInput::avr::AvrAdcAxes<0, 1>,   // X=A0, Y=A1
 *     hw::avr::AVRAdc<>
 *   >;
 *   InDef<oneMenu::JoyIn<JoyHW>, oneMenu::BtnIn<SwHW>, PCKbd> in;
 *
 * Menu adapters (EncIn, BtnIn, JoyIn) live in OneMenu: oneMenu/menu/IO/IOP/encIn.h / btnIn.h / joyIn.h
 */

#include <oneInput/inputDef.h>
#include <oneInput/debounce.h>
#include <oneInput/click.h>
#include <oneInput/hold.h>
#include <oneInput/encoder.h>
#include <oneInput/btnCapture.h>
#include <oneInput/joystick.h>

#ifdef __AVR__
  #include <oneInput/avr/avrBtnPin.h>
  #include <oneInput/avr/avrEncPins.h>
  #include <oneInput/avr/avrAdcAxes.h>
#elif defined(ESP32) || defined(ESP8266)
  #include <oneInput/esp32/esp32BtnPin.h>
  #include <oneInput/esp32/esp32EncPins.h>
#endif
