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

#include <GLFW/glfw3.h>
#include <optional>

#include "src/events/key.h"
#include "src/events/mouse.h"

#include "src/platform/keyconversion.h"

namespace Platform {

Events::Key convertGLFW2Key(int key) {
	switch (key) {
		case GLFW_KEY_SPACE:         return Events::kKeySpace;
		case GLFW_KEY_APOSTROPHE:    return Events::kKeyApostrophe;
		case GLFW_KEY_COMMA:         return Events::kKeyComma;
		case GLFW_KEY_MINUS:         return Events::kKeyMinus;
		case GLFW_KEY_PERIOD:        return Events::kKeyPeriod;
		case GLFW_KEY_SLASH:         return Events::kKeySlash;

		case GLFW_KEY_0:             return Events::kKey0;
		case GLFW_KEY_1:             return Events::kKey1;
		case GLFW_KEY_2:             return Events::kKey2;
		case GLFW_KEY_3:             return Events::kKey3;
		case GLFW_KEY_4:             return Events::kKey4;
		case GLFW_KEY_5:             return Events::kKey5;
		case GLFW_KEY_6:             return Events::kKey6;
		case GLFW_KEY_7:             return Events::kKey7;
		case GLFW_KEY_8:             return Events::kKey8;
		case GLFW_KEY_9:             return Events::kKey9;

		case GLFW_KEY_SEMICOLON:     return Events::kKeySemicolon;
		case GLFW_KEY_EQUAL:         return Events::kKeyEqual;

		case GLFW_KEY_A:             return Events::kKeyA;
		case GLFW_KEY_B:             return Events::kKeyB;
		case GLFW_KEY_C:             return Events::kKeyC;
		case GLFW_KEY_D:             return Events::kKeyD;
		case GLFW_KEY_E:             return Events::kKeyE;
		case GLFW_KEY_F:             return Events::kKeyF;
		case GLFW_KEY_G:             return Events::kKeyG;
		case GLFW_KEY_H:             return Events::kKeyH;
		case GLFW_KEY_I:             return Events::kKeyI;
		case GLFW_KEY_J:             return Events::kKeyJ;
		case GLFW_KEY_K:             return Events::kKeyK;
		case GLFW_KEY_L:             return Events::kKeyL;
		case GLFW_KEY_M:             return Events::kKeyM;
		case GLFW_KEY_N:             return Events::kKeyN;
		case GLFW_KEY_O:             return Events::kKeyO;
		case GLFW_KEY_P:             return Events::kKeyP;
		case GLFW_KEY_Q:             return Events::kKeyQ;
		case GLFW_KEY_R:             return Events::kKeyR;
		case GLFW_KEY_S:             return Events::kKeyS;
		case GLFW_KEY_T:             return Events::kKeyT;
		case GLFW_KEY_U:             return Events::kKeyU;
		case GLFW_KEY_V:             return Events::kKeyV;
		case GLFW_KEY_W:             return Events::kKeyW;
		case GLFW_KEY_X:             return Events::kKeyX;
		case GLFW_KEY_Y:             return Events::kKeyY;
		case GLFW_KEY_Z:             return Events::kKeyZ;

		case GLFW_KEY_LEFT_BRACKET:  return Events::kKeyLeftBracket;
		case GLFW_KEY_BACKSLASH:     return Events::kKeyBackslash;
		case GLFW_KEY_RIGHT_BRACKET: return Events::kKeyRightBracket;
		case GLFW_KEY_GRAVE_ACCENT:  return Events::kKeyGraveAccent;

		case GLFW_KEY_WORLD_1:       return Events::kKeyWorld1;
		case GLFW_KEY_WORLD_2:       return Events::kKeyWorld2;

		case GLFW_KEY_ESCAPE:        return Events::kKeyEscape;
		case GLFW_KEY_ENTER:         return Events::kKeyEnter;
		case GLFW_KEY_TAB:           return Events::kKeyTab;
		case GLFW_KEY_BACKSPACE:     return Events::kKeyBackspace;

		case GLFW_KEY_INSERT:        return Events::kKeyInsert;
		case GLFW_KEY_DELETE:        return Events::kKeyDelete;

		case GLFW_KEY_RIGHT:         return Events::kKeyRight;
		case GLFW_KEY_LEFT:          return Events::kKeyLeft;
		case GLFW_KEY_DOWN:          return Events::kKeyDown;
		case GLFW_KEY_UP:            return Events::kKeyUp;

		case GLFW_KEY_PAGE_UP:       return Events::kKeyPageUp;
		case GLFW_KEY_PAGE_DOWN:     return Events::kKeyPageDown;

		case GLFW_KEY_HOME:          return Events::kKeyHome;
		case GLFW_KEY_END:           return Events::kKeyEnd;
		case GLFW_KEY_CAPS_LOCK:     return Events::kKeyCapsLock;
		case GLFW_KEY_SCROLL_LOCK:   return Events::kKeyScrollLock;
		case GLFW_KEY_NUM_LOCK:      return Events::kKeyNumLock;
		case GLFW_KEY_PRINT_SCREEN:  return Events::kKeyPrintScreen;
		case GLFW_KEY_PAUSE:         return Events::kKeyPause;

		case GLFW_KEY_F1:            return Events::kKeyF1;
		case GLFW_KEY_F2:            return Events::kKeyF2;
		case GLFW_KEY_F3:            return Events::kKeyF3;
		case GLFW_KEY_F4:            return Events::kKeyF4;
		case GLFW_KEY_F5:            return Events::kKeyF5;
		case GLFW_KEY_F6:            return Events::kKeyF6;
		case GLFW_KEY_F7:            return Events::kKeyF7;
		case GLFW_KEY_F8:            return Events::kKeyF8;
		case GLFW_KEY_F9:            return Events::kKeyF9;
		case GLFW_KEY_F11:           return Events::kKeyF11;
		case GLFW_KEY_F12:           return Events::kKeyF12;
		case GLFW_KEY_F13:           return Events::kKeyF13;
		case GLFW_KEY_F14:           return Events::kKeyF14;
		case GLFW_KEY_F15:           return Events::kKeyF15;
		case GLFW_KEY_F16:           return Events::kKeyF16;
		case GLFW_KEY_F17:           return Events::kKeyF17;
		case GLFW_KEY_F18:           return Events::kKeyF18;
		case GLFW_KEY_F19:           return Events::kKeyF19;
		case GLFW_KEY_F20:           return Events::kKeyF20;
		case GLFW_KEY_F21:           return Events::kKeyF21;
		case GLFW_KEY_F22:           return Events::kKeyF22;
		case GLFW_KEY_F23:           return Events::kKeyF23;
		case GLFW_KEY_F24:           return Events::kKeyF24;

		case GLFW_KEY_KP_0:          return Events::kKeyKeypad0;
		case GLFW_KEY_KP_1:          return Events::kKeyKeypad1;
		case GLFW_KEY_KP_2:          return Events::kKeyKeypad2;
		case GLFW_KEY_KP_3:          return Events::kKeyKeypad3;
		case GLFW_KEY_KP_4:          return Events::kKeyKeypad4;
		case GLFW_KEY_KP_5:          return Events::kKeyKeypad5;
		case GLFW_KEY_KP_6:          return Events::kKeyKeypad6;
		case GLFW_KEY_KP_7:          return Events::kKeyKeypad7;
		case GLFW_KEY_KP_8:          return Events::kKeyKeypad8;
		case GLFW_KEY_KP_9:          return Events::kKeyKeypad9;

		case GLFW_KEY_KP_DECIMAL:    return Events::kKeyKeypadDecimal;
		case GLFW_KEY_KP_DIVIDE:     return Events::kKeyKeypadDivide;
		case GLFW_KEY_KP_MULTIPLY:   return Events::kKeyKeypadMultiply;
		case GLFW_KEY_KP_SUBTRACT:   return Events::kKeyKeypadSubtract;
		case GLFW_KEY_KP_ADD:        return Events::kKeyKeypadAdd;
		case GLFW_KEY_KP_ENTER:      return Events::kKeyKeypadEnter;
		case GLFW_KEY_KP_EQUAL:      return Events::kKeyKeypadEqual;

		case GLFW_KEY_LEFT_SHIFT:    return Events::kKeyLeftShift;
		case GLFW_KEY_LEFT_ALT:      return Events::kKeyLeftAlt;
		case GLFW_KEY_LEFT_CONTROL:  return Events::kKeyLeftControl;
		case GLFW_KEY_LEFT_SUPER:    return Events::kKeyLeftSuper;

		case GLFW_KEY_RIGHT_SHIFT:    return Events::kKeyRightShift;
		case GLFW_KEY_RIGHT_ALT:      return Events::kKeyRightAlt;
		case GLFW_KEY_RIGHT_CONTROL:  return Events::kKeyRightControl;
		case GLFW_KEY_RIGHT_SUPER:    return Events::kKeyRightSuper;

		case GLFW_KEY_MENU:           return Events::kKeyMenu;

		case GLFW_KEY_UNKNOWN:       [[fallthrough]];
		default:
			return Events::kKeyUnknown;
	}
}

Events::Key convertScan12Key(WindowsScanCode key) {
	if (key.second.has_value()) {
		switch (key.second.value()) {
			case kWindowsVKPause: return Events::kKeyPause;
			case kWindowsVKMenu: return Events::kKeyMenu;
			case kWindowsVKKPDecimal: return Events::kKeyKeypadDecimal;
			case kWindowsVKKP0: return Events::kKeyKeypad0;
			case kWindowsVKKP1: return Events::kKeyKeypad1;
			case kWindowsVKKP2: return Events::kKeyKeypad2;
			case kWindowsVKKP3: return Events::kKeyKeypad3;
			case kWindowsVKKP4: return Events::kKeyKeypad4;
			case kWindowsVKKP5: return Events::kKeyKeypad5;
			case kWindowsVKKP6: return Events::kKeyKeypad6;
			case kWindowsVKKP7: return Events::kKeyKeypad7;
			case kWindowsVKKP8: return Events::kKeyKeypad8;
			case kWindowsVKKP9: return Events::kKeyKeypad9;
			default: return Events::kKeyUnknown;
		}
	}
	switch (key.first) {
		case kScan1A: return Events::kKeyA;
		case kScan1B: return Events::kKeyB;
		case kScan1C: return Events::kKeyC;
		case kScan1D: return Events::kKeyD;
		case kScan1E: return Events::kKeyE;
		case kScan1F: return Events::kKeyF;
		case kScan1G: return Events::kKeyG;
		case kScan1H: return Events::kKeyH;
		case kScan1I: return Events::kKeyI;
		case kScan1J: return Events::kKeyJ;
		case kScan1K: return Events::kKeyK;
		case kScan1L: return Events::kKeyL;
		case kScan1M: return Events::kKeyM;
		case kScan1N: return Events::kKeyN;
		case kScan1O: return Events::kKeyO;
		case kScan1P: return Events::kKeyP;
		case kScan1Q: return Events::kKeyQ;
		case kScan1R: return Events::kKeyR;
		case kScan1S: return Events::kKeyS;
		case kScan1T: return Events::kKeyT;
		case kScan1U: return Events::kKeyU;
		case kScan1V: return Events::kKeyV;
		case kScan1W: return Events::kKeyW;
		case kScan1X: return Events::kKeyX;
		case kScan1Y: return Events::kKeyY;
		case kScan1Z: return Events::kKeyZ;

		case kScan1F1:  return Events::kKeyF1;
		case kScan1F2:  return Events::kKeyF2;
		case kScan1F3:  return Events::kKeyF3;
		case kScan1F4:  return Events::kKeyF4;
		case kScan1F5:  return Events::kKeyF5;
		case kScan1F6:  return Events::kKeyF6;
		case kScan1F7:  return Events::kKeyF7;
		case kScan1F8:  return Events::kKeyF8;
		case kScan1F9:  return Events::kKeyF9;
		case kScan1F10: return Events::kKeyF10;
		case kScan1F11: return Events::kKeyF11;
		case kScan1F12: return Events::kKeyF12;
		case kScan1F13: return Events::kKeyF13;
		case kScan1F14: return Events::kKeyF14;
		case kScan1F15: return Events::kKeyF15;
		case kScan1F16: return Events::kKeyF16;
		case kScan1F17: return Events::kKeyF17;
		case kScan1F18: return Events::kKeyF18;
		case kScan1F19: return Events::kKeyF19;
		case kScan1F20: return Events::kKeyF20;
		case kScan1F21: return Events::kKeyF21;
		case kScan1F22: return Events::kKeyF22;
		case kScan1F23: return Events::kKeyF23;
		case kScan1F24: return Events::kKeyF24;

		case kScan11: return Events::kKey1;
		case kScan12: return Events::kKey2;
		case kScan13: return Events::kKey3;
		case kScan14: return Events::kKey4;
		case kScan15: return Events::kKey5;
		case kScan16: return Events::kKey6;
		case kScan17: return Events::kKey7;
		case kScan18: return Events::kKey8;
		case kScan19: return Events::kKey9;
		case kScan10: return Events::kKey0;

		case kScan1KPNumLock:  return Events::kKeyNumLock;
		case kScan1KPMultiply: return Events::kKeyKeypadMultiply;
		case kScan1KPMinus:    return Events::kKeyKeypadSubtract;
		case kScan1KPPlus:	   return Events::kKeyKeypadAdd;

		case kScan1Escape:		 return Events::kKeyEscape;
		case kScan1GraveAccent:  return Events::kKeyGraveAccent;
		case kScan1Tab: 		 return Events::kKeyTab;
		case kScan1CapsLock:	 return Events::kKeyCapsLock;

		case kScan1LeftShift:   return Events::kKeyLeftShift;
		case kScan1LeftControl: return Events::kKeyLeftControl;
		case kScan1LeftAlt:		return Events::kKeyLeftAlt;
		case kScan1LeftSuper:   return Events::kKeyLeftSuper;
		case kScan1RightShift:  return Events::kKeyRightShift;
		case kScan1RightSuper:  return Events::kKeyRightSuper;

		case kScan1Space:  return Events::kKeySpace;
		case kScan1Comma:  return Events::kKeyComma;
		case kScan1Period: return Events::kKeyPeriod;
		case kScan1Slash:  return Events::kKeySlash;

		case kScan1Dash:		 return Events::kKeyMinus;
		case kScan1Equals:		 return Events::kKeyEqual;
		case kScan1Backspace:	 return Events::kKeyBackspace;
		case kScan1Enter:		 return Events::kKeyEnter;
		case kScan1LeftBracket:  return Events::kKeyLeftBracket;
		case kScan1RightBracket: return Events::kKeyRightBracket;
		case kScan1Apostrophe:   return Events::kKeyApostrophe;
		case kScan1Semicolon:	 return Events::kKeySemicolon;

		case kScan1Insert:		return Events::kKeyInsert;
		case kScan1Home:		return Events::kKeyHome;
		case kScan1End:			return Events::kKeyEnd;
		case kScan1PageUp:		return Events::kKeyPageUp;
		case kScan1PageDown:	return Events::kKeyPageDown;
		case kScan1Delete:		return Events::kKeyDelete;
		case kScan1PrintScreen: return Events::kKeyPrintScreen;
		case kScan1ScrollLock:  return Events::kKeyScrollLock;

		case kScan1Left:  return Events::kKeyLeft;
		case kScan1Right: return Events::kKeyRight;
		case kScan1Up:    return Events::kKeyUp;
		case kScan1Down:  return Events::kKeyDown;

		case kScan1World1: return Events::kKeyWorld1;
		case kScan1World2: return Events::kKeyWorld2;

		default:
			return Events::kKeyUnknown;
	}
}

tl::expected<WindowsScanCode, Error*> convertKey2Scan(Events::Key key) {
	switch (key) {
		case Events::kKeyKeypadDecimal: return std::make_pair(kScan1KPDecimal, kWindowsVKKPDecimal);
		case Events::kKeyKeypad0: return std::make_pair(kScan1KP0, kWindowsVKKP0);
		case Events::kKeyKeypad1: return std::make_pair(kScan1KP1, kWindowsVKKP1);
		case Events::kKeyKeypad2: return std::make_pair(kScan1KP2, kWindowsVKKP2);
		case Events::kKeyKeypad3: return std::make_pair(kScan1KP3, kWindowsVKKP3);
		case Events::kKeyKeypad4: return std::make_pair(kScan1KP4, kWindowsVKKP4);
		case Events::kKeyKeypad5: return std::make_pair(kScan1KP5, kWindowsVKKP5);
		case Events::kKeyKeypad6: return std::make_pair(kScan1KP6, kWindowsVKKP6);
		case Events::kKeyKeypad7: return std::make_pair(kScan1KP7, kWindowsVKKP7);
		case Events::kKeyKeypad8: return std::make_pair(kScan1KP8, kWindowsVKKP8);
		case Events::kKeyKeypad9: return std::make_pair(kScan1KP9, kWindowsVKKP9);
		case Events::kKeyPause: return std::make_pair(kScan1Pause, kWindowsVKPause);
		case Events::kKeyMenu: return std::make_pair(kScan1Menu, kWindowsVKMenu);

		case Events::kKeyA: return std::make_pair(kScan1A, std::nullopt);
		case Events::kKeyB: return std::make_pair(kScan1B, std::nullopt);
		case Events::kKeyC: return std::make_pair(kScan1C, std::nullopt);
		case Events::kKeyD: return std::make_pair(kScan1D, std::nullopt);
		case Events::kKeyE: return std::make_pair(kScan1E, std::nullopt);
		case Events::kKeyF: return std::make_pair(kScan1F, std::nullopt);
		case Events::kKeyG: return std::make_pair(kScan1G, std::nullopt);
		case Events::kKeyH: return std::make_pair(kScan1H, std::nullopt);
		case Events::kKeyI: return std::make_pair(kScan1I, std::nullopt);
		case Events::kKeyJ: return std::make_pair(kScan1J, std::nullopt);
		case Events::kKeyK: return std::make_pair(kScan1K, std::nullopt);
		case Events::kKeyL: return std::make_pair(kScan1L, std::nullopt);
		case Events::kKeyM: return std::make_pair(kScan1M, std::nullopt);
		case Events::kKeyN: return std::make_pair(kScan1N, std::nullopt);
		case Events::kKeyO: return std::make_pair(kScan1O, std::nullopt);
		case Events::kKeyP: return std::make_pair(kScan1P, std::nullopt);
		case Events::kKeyQ: return std::make_pair(kScan1Q, std::nullopt);
		case Events::kKeyR: return std::make_pair(kScan1R, std::nullopt);
		case Events::kKeyS: return std::make_pair(kScan1S, std::nullopt);
		case Events::kKeyT: return std::make_pair(kScan1T, std::nullopt);
		case Events::kKeyU: return std::make_pair(kScan1U, std::nullopt);
		case Events::kKeyV: return std::make_pair(kScan1V, std::nullopt);
		case Events::kKeyW: return std::make_pair(kScan1W, std::nullopt);
		case Events::kKeyX: return std::make_pair(kScan1X, std::nullopt);
		case Events::kKeyY: return std::make_pair(kScan1Y, std::nullopt);
		case Events::kKeyZ: return std::make_pair(kScan1Z, std::nullopt);

		case Events::kKeyF1:  return std::make_pair(kScan1F1, std::nullopt);
		case Events::kKeyF2:  return std::make_pair(kScan1F2, std::nullopt);
		case Events::kKeyF3:  return std::make_pair(kScan1F3, std::nullopt);
		case Events::kKeyF4:  return std::make_pair(kScan1F4, std::nullopt);
		case Events::kKeyF5:  return std::make_pair(kScan1F5, std::nullopt);
		case Events::kKeyF6:  return std::make_pair(kScan1F6, std::nullopt);
		case Events::kKeyF7:  return std::make_pair(kScan1F7, std::nullopt);
		case Events::kKeyF8:  return std::make_pair(kScan1F8, std::nullopt);
		case Events::kKeyF9:  return std::make_pair(kScan1F9, std::nullopt);
		case Events::kKeyF10: return std::make_pair(kScan1F10, std::nullopt);
		case Events::kKeyF11: return std::make_pair(kScan1F11, std::nullopt);
		case Events::kKeyF12: return std::make_pair(kScan1F12, std::nullopt);
		case Events::kKeyF13: return std::make_pair(kScan1F13, std::nullopt);
		case Events::kKeyF14: return std::make_pair(kScan1F14, std::nullopt);
		case Events::kKeyF15: return std::make_pair(kScan1F15, std::nullopt);
		case Events::kKeyF16: return std::make_pair(kScan1F16, std::nullopt);
		case Events::kKeyF17: return std::make_pair(kScan1F17, std::nullopt);
		case Events::kKeyF18: return std::make_pair(kScan1F18, std::nullopt);
		case Events::kKeyF19: return std::make_pair(kScan1F19, std::nullopt);
		case Events::kKeyF20: return std::make_pair(kScan1F20, std::nullopt);
		case Events::kKeyF21: return std::make_pair(kScan1F21, std::nullopt);
		case Events::kKeyF22: return std::make_pair(kScan1F22, std::nullopt);
		case Events::kKeyF23: return std::make_pair(kScan1F23, std::nullopt);
		case Events::kKeyF24: return std::make_pair(kScan1F24, std::nullopt);

		case Events::kKey1: return std::make_pair(kScan11, std::nullopt);
		case Events::kKey2: return std::make_pair(kScan12, std::nullopt);
		case Events::kKey3: return std::make_pair(kScan13, std::nullopt);
		case Events::kKey4: return std::make_pair(kScan14, std::nullopt);
		case Events::kKey5: return std::make_pair(kScan15, std::nullopt);
		case Events::kKey6: return std::make_pair(kScan16, std::nullopt);
		case Events::kKey7: return std::make_pair(kScan17, std::nullopt);
		case Events::kKey8: return std::make_pair(kScan18, std::nullopt);
		case Events::kKey9: return std::make_pair(kScan19, std::nullopt);
		case Events::kKey0: return std::make_pair(kScan10, std::nullopt);

		case Events::kKeyNumLock:		 return std::make_pair(kScan1KPNumLock, std::nullopt);
		case Events::kKeyKeypadDivide: 	 return std::make_pair(kScan1KPDivide, std::nullopt);
		case Events::kKeyKeypadMultiply: return std::make_pair(kScan1KPMultiply, std::nullopt);
		case Events::kKeyKeypadSubtract: return std::make_pair(kScan1KPMinus, std::nullopt);
		case Events::kKeyKeypadAdd: 	 return std::make_pair(kScan1KPPlus, std::nullopt);
		case Events::kKeyKeypadEnter: 	 return std::make_pair(kScan1KPEnter, std::nullopt);

		case Events::kKeyEscape: 		return std::make_pair(kScan1Escape, std::nullopt);
		case Events::kKeyGraveAccent: 	return std::make_pair(kScan1GraveAccent, std::nullopt);
		case Events::kKeyTab: 			return std::make_pair(kScan1Tab, std::nullopt);
		case Events::kKeyCapsLock: 		return std::make_pair(kScan1CapsLock, std::nullopt);

		case Events::kKeyLeftShift: 	return std::make_pair(kScan1LeftShift, std::nullopt);
		case Events::kKeyLeftControl: 	return std::make_pair(kScan1LeftControl, std::nullopt);
		case Events::kKeyLeftAlt: 		return std::make_pair(kScan1LeftAlt, std::nullopt);
		case Events::kKeyLeftSuper: 	return std::make_pair(kScan1LeftSuper, std::nullopt);

		case Events::kKeyRightShift:	return std::make_pair(kScan1RightShift, std::nullopt);
		case Events::kKeyRightSuper: 	return std::make_pair(kScan1RightSuper, std::nullopt);

		case Events::kKeySpace: 		return std::make_pair(kScan1Space, std::nullopt);
		case Events::kKeyComma: 		return std::make_pair(kScan1Comma, std::nullopt);
		case Events::kKeyPeriod: 		return std::make_pair(kScan1Period, std::nullopt);
		case Events::kKeySlash: 		return std::make_pair(kScan1Slash, std::nullopt);

		case Events::kKeyMinus: 		return std::make_pair(kScan1Dash, std::nullopt);
		case Events::kKeyEqual: 		return std::make_pair(kScan1Equals, std::nullopt);
		case Events::kKeyBackspace: 	return std::make_pair(kScan1Backspace, std::nullopt);
		case Events::kKeyEnter: 		return std::make_pair(kScan1Enter, std::nullopt);
		case Events::kKeyLeftBracket: 	return std::make_pair(kScan1LeftBracket, std::nullopt);
		case Events::kKeyRightBracket: 	return std::make_pair(kScan1RightBracket, std::nullopt);
		case Events::kKeyApostrophe: 	return std::make_pair(kScan1Apostrophe, std::nullopt);
		case Events::kKeySemicolon: 	return std::make_pair(kScan1Semicolon, std::nullopt);

		case Events::kKeyInsert: 		return std::make_pair(kScan1Insert, std::nullopt);
		case Events::kKeyHome: 			return std::make_pair(kScan1Home, std::nullopt);
		case Events::kKeyEnd: 			return std::make_pair(kScan1End, std::nullopt);
		case Events::kKeyPageUp: 		return std::make_pair(kScan1PageUp, std::nullopt);
		case Events::kKeyPageDown: 		return std::make_pair(kScan1PageDown, std::nullopt);
		case Events::kKeyDelete: 		return std::make_pair(kScan1Delete, std::nullopt);
		case Events::kKeyPrintScreen: 	return std::make_pair(kScan1PrintScreen, std::nullopt);
		case Events::kKeyScrollLock: 	return std::make_pair(kScan1ScrollLock, std::nullopt);

		case Events::kKeyLeft: 			return std::make_pair(kScan1Left, std::nullopt);
		case Events::kKeyRight: 		return std::make_pair(kScan1Right, std::nullopt);
		case Events::kKeyUp: 			return std::make_pair(kScan1Up, std::nullopt);
		case Events::kKeyDown: 			return std::make_pair(kScan1Down, std::nullopt);

		case Events::kKeyWorld1: 		return std::make_pair(kScan1World1, std::nullopt);
		case Events::kKeyWorld2: 		return std::make_pair(kScan1World2, std::nullopt);

		case Events::kKeyUnknown: [[fallthrough]];
		default:
			return tl::unexpected(new Error(ErrorMessage("Cannot convert unknown scan code"))); 
	}
}

enum MouseScanButton {
	kMouseButton1 = 0x100,
	kMouseButton2 = 0x101,
	kMouseButton3 = 0x102,
	kMouseButton4 = 0x103,
	kMouseButton5 = 0x104
};

tl::expected<Events::MouseButton, Error*> convertScan2MouseButton(WindowsScanCode key) {
	switch (key.first) {
		case kMouseButton1: return Events::kMouseButton1;
		case kMouseButton2: return Events::kMouseButton2;
		case kMouseButton3: return Events::kMouseButton3;
		case kMouseButton4: return Events::kMouseButton4;
		case kMouseButton5: return Events::kMouseButton5;
		default:
			return tl::unexpected(new Error(ErrorMessage("Cannot convert scan code {} to a mouse button", key.first)));
	}
}

tl::expected<WindowsScanCode, Error*> convertMouseButton2Scan(Events::MouseButton button) {
	switch (button) {
		case Events::kMouseButton1: return std::make_pair(kMouseButton1, std::nullopt);
		case Events::kMouseButton2: return std::make_pair(kMouseButton2, std::nullopt);
		case Events::kMouseButton3: return std::make_pair(kMouseButton3, std::nullopt);
		case Events::kMouseButton4: return std::make_pair(kMouseButton4, std::nullopt);
		case Events::kMouseButton5: return std::make_pair(kMouseButton5, std::nullopt);
		default:
			return tl::unexpected(new Error(ErrorMessage("Cannot convert mouse button {} to a recognisable value", (int)button)));
	}
}

} // End of namespace Platform
