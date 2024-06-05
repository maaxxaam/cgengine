/* OpenAWE - A reimplementation of Remedys Alan Wake Engine
 *
 * OpenAWE is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * OpenAWE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * OpenAWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenAWE. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENAWE_KEYCONVERSION_H
#define OPENAWE_KEYCONVERSION_H

#include <optional>
#include <utility>

#include "expected.hpp"
#include "src/error.h"

#include "src/events/key.h"
#include "src/events/mouse.h"

namespace Platform {

enum WindowsVirtualCode {
	kWindowsVKPause = 0x13,
	kWindowsVKMenu = 0x5d,
	kWindowsVKKPDecimal = 0x6e,
	kWindowsVKKP0 = 0x60,
	kWindowsVKKP1 = 0x61,
	kWindowsVKKP2 = 0x62,
	kWindowsVKKP3 = 0x63,
	kWindowsVKKP4 = 0x64,
	kWindowsVKKP5 = 0x65,
	kWindowsVKKP6 = 0x66,
	kWindowsVKKP7 = 0x67,
	kWindowsVKKP8 = 0x68,
	kWindowsVKKP9 = 0x69
};

/*
 * Note: these are not the actual Scan 1 Make codes but values used in configs
 * for OG AW and AWAN, which follow them closely. Values contain duplicates:
 * either because of omitting the extended bit (0xE0/0xE1), adhering to 
 * the GLFW naming convention or AWE specific bindings (as with NumLock/Pause).
 * For more info on Scan 1 Make codes, see MS keyboard scan code specification.
 */
enum Scan1MakeCode {
	kScan1A = 0x1e,
	kScan1B = 0x30,
	kScan1C = 0x2e,
	kScan1D = 0x20,
	kScan1E = 0x12,
	kScan1F = 0x21,
	kScan1G = 0x22,
	kScan1H = 0x23,
	kScan1I = 0x17,
	kScan1J = 0x24,
	kScan1K = 0x25,
	kScan1L = 0x26,
	kScan1M = 0x32,
	kScan1N = 0x31,
	kScan1O = 0x18,
	kScan1P = 0x19,
	kScan1Q = 0x10,
	kScan1R = 0x13,
	kScan1S = 0x1f,
	kScan1T = 0x14,
	kScan1U = 0x16,
	kScan1V = 0x2f,
	kScan1W = 0x11,
	kScan1X = 0x2d,
	kScan1Y = 0x15,
	kScan1Z = 0x2c,

	kScan1F1 = 0x3b,
	kScan1F2 = 0x3c,
	kScan1F3 = 0x3d,
	kScan1F4 = 0x3e,
	kScan1F5 = 0x3f,
	kScan1F6 = 0x40,
	kScan1F7 = 0x41,
	kScan1F8 = 0x42,
	kScan1F9 = 0x43,
	kScan1F10 = 0x44,
	kScan1F11 = 0x57,
	kScan1F12 = 0x58,
	kScan1F13 = 0x64,
	kScan1F14 = 0x65,
	kScan1F15 = 0x66,
	kScan1F16 = 0x67,
	kScan1F17 = 0x68,
	kScan1F18 = 0x69,
	kScan1F19 = 0x6a,
	kScan1F20 = 0x6b,
	kScan1F21 = 0x6c,
	kScan1F22 = 0x6d,
	kScan1F23 = 0x6e,
	kScan1F24 = 0x76,

	kScan11 = 0x02,
	kScan12 = 0x03,
	kScan13 = 0x04,
	kScan14 = 0x05,
	kScan15 = 0x06,
	kScan16 = 0x07,
	kScan17 = 0x08,
	kScan18 = 0x09,
	kScan19 = 0x0a,
	kScan10 = 0x0b,

	kScan1KP1 = 0x4f,
	kScan1KP2 = 0x50,
	kScan1KP3 = 0x51,
	kScan1KP4 = 0x4b,
	kScan1KP5 = 0x4c,
	kScan1KP6 = 0x4d,
	kScan1KP7 = 0x47,
	kScan1KP8 = 0x48,
	kScan1KP9 = 0x49,
	kScan1KP0 = 0x52,

	kScan1KPNumLock = 0x45,
	kScan1KPDecimal = 0x53,
	kScan1KPDivide = 0x35,
	kScan1KPMultiply = 0x37,
	kScan1KPMinus = 0x4a,
	kScan1KPPlus = 0x4e,
	kScan1KPEnter = 0x1c,

	kScan1Escape = 0x01,
	kScan1GraveAccent = 0x29,
	kScan1Tab = 0x0f,

	kScan1CapsLock = 0x3a,
	kScan1LeftShift = 0x2a,
	kScan1LeftControl = 0x1d,
	kScan1LeftAlt = 0x38,
	kScan1LeftSuper = 0x5b,

	kScan1RightShift = 0x36,
	kScan1RightSuper = 0x5c,

	kScan1Space = 0x39,
	kScan1Comma = 0x33,
	kScan1Period = 0x34,
	kScan1Slash = 0x35,

	kScan1Dash = 0x0c,
	kScan1Equals = 0x0d,
	kScan1Backspace = 0x0e,
	kScan1Enter = 0x1c,
	kScan1LeftBracket = 0x1a,
	kScan1RightBracket = 0x1b,
	kScan1Apostrophe = 0x28,
	kScan1Semicolon = 0x27,

	kScan1Insert = 0x52,
	kScan1Home = 0x47,
	kScan1End = 0x4f,
	kScan1PageUp = 0x49,
	kScan1PageDown = 0x51,
	kScan1Delete = 0x4c,
	kScan1Pause = 0x45,
	kScan1PrintScreen = 0x54,
	kScan1ScrollLock = 0x46,

	kScan1Left = 0x4b,
	kScan1Right = 0x4d,
	kScan1Up = 0x48,
	kScan1Down = 0x50,

	kScan1World1 = 0x2b,
	kScan1World2 = 0x56,
	kScan1Menu = 0x5d,
};

/*! 
 * Windows scan code consists of "Scan 1 Make" code (0x0104 max)
 * and an optional Windows virtual key code (one byte)
 */
typedef std::pair<short, std::optional<char>> WindowsScanCode;

Events::Key convertGLFW2Key(int key);

Events::Key convertScan12Key(WindowsScanCode key);

tl::expected<WindowsScanCode, Error*> convertKey2Scan(Events::Key key);

tl::expected<Events::MouseButton, Error*> convertScan2MouseButton(WindowsScanCode key);

tl::expected<WindowsScanCode, Error*> convertMouseButton2Scan(Events::MouseButton key);

} // End of namespace Platform

#endif //OPENAWE_KEYCONVERSION_H
