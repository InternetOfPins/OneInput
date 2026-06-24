#pragma once

/**
 * @file oneInput.h
 * @brief OneInput — composable physical input library for IOP.
 *
 * Components assemble via hapi::APIOf — outermost (menu adapter) to innermost (ISR attachment):
 *
 *   // Encoder on PC0+PC1, 4 steps/detent → CKE Up/Down
 *   using Enc = hapi::APIOf<oneInput::InputDef,
 *     oneMenu::EncIn<4>,
 *     oneInput::Encoder,
 *     oneInput::avr::AvrEncPins<1, chip::PortC, 0, 1>
 *   >;
 *
 *   // Button on PC2, click→Enter, hold→Esc
 *   using Btn = hapi::APIOf<oneInput::InputDef,
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
 *   using EncHW = hapi::APIOf<oneInput::InputDef,
 *     oneInput::Encoder,
 *     oneInput::Esp32EncPins<34, 35>   // CLK=34, DT=35
 *   >;
 *   using BtnHW = hapi::APIOf<oneInput::InputDef,
 *     oneInput::BtnCapture,
 *     oneInput::Hold<800>,
 *     oneInput::Click<300>,
 *     oneInput::Debounce<20>,
 *     oneInput::Esp32BtnPin<32>
 *   >;
 *   // No ISR needed — begin() wires attachInterrupt
 *   InDef<oneMenu::EncIn<EncHW, 4>, oneMenu::BtnIn<BtnHW>, PCKbd> in;
 *
 * Menu adapters (EncIn, BtnIn) live in OneMenu: oneMenu/menu/IO/IOP/encIn.h / btnIn.h
 */

#include <oneInput/inputDef.h>
#include <oneInput/debounce.h>
#include <oneInput/click.h>
#include <oneInput/hold.h>
#include <oneInput/encoder.h>
#include <oneInput/btnCapture.h>

#ifdef __AVR__
  #include <oneInput/avr/avrBtnPin.h>
  #include <oneInput/avr/avrEncPins.h>
#elif defined(ESP32) || defined(ESP8266)
  #include <oneInput/esp32/esp32BtnPin.h>
  #include <oneInput/esp32/esp32EncPins.h>
#endif
