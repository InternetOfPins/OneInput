/**
 * @file inputEvent.h
 * @brief Unified input command enumeration for IOP input layer.
 *
 * Defines Cmd enum used by all input sources (hardware drivers, filters, adapters).
 * OneMenu wraps Cmd in CKE struct with menu-specific metadata (key, ext, kbd).
 *
 * Usage:
 *   oneInput::Cmd c = oneInput::Cmd::Up;    // input drivers
 *   oneMenu::CKE evt{c};                     // menu system
 */

#pragma once

namespace oneInput {

  /// Unified input command type: rising edge from any input source
  enum class Cmd : uint8_t {
    None  = 0,
    Enter = 1<<0,
    Esc   = 1<<1,
    Up    = 1<<2,
    Down  = 1<<3,
    Left  = 1<<4,
    Right = 1<<5,
    Key   = 1<<6,
    Go    = 1<<7
  };

  // Bitwise operators for Cmd (if needed in future)
  inline constexpr Cmd operator|(Cmd a, Cmd b) {
    return static_cast<Cmd>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
  }

  inline constexpr Cmd operator&(Cmd a, Cmd b) {
    return static_cast<Cmd>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
  }

} // oneInput
