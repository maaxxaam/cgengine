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

#include "eventman.h"
#include "src/events/gamepad.h"
#include "src/events/key.h"
#include "src/events/mouse.h"

#include <cstdint>

namespace Events {

inline void EventManager::invokeCallbacks(const EventData data, const uint32_t action) {
	const Event event{action, data};
	auto callbacks = _actionCallbacks.equal_range(action);
	while (callbacks.first != callbacks.second) {
		EventCallback action = ((callbacks.first)->second);
		action(event);
		callbacks.first++;
	};
}

void EventManager::injectKeyboardInput(const Events::Key key, const Events::KeyState state, const uint32_t modifiers) {
	KeyCombination combination = std::make_pair(key, modifiers);
	const auto action = _keyBindings.map()->find(combination);
	if (action == _keyBindings.map()->cend()) return;

	invokeCallbacks(KeyEvent{state}, action->second);
}

void EventManager::injectMouseButtonInput(const Events::MouseButton mouse, const Events::KeyState state) {
	const auto action = _mouseBindings.map()->find(mouse);
	if (action == _mouseBindings.map()->cend()) return;

	invokeCallbacks(KeyEvent{state}, action->second);
}

void EventManager::injectGamepadButtonInput(const Events::GamepadButton button, const Events::KeyState state) {
	const auto action = _gamepadBindings.map()->find(button);
	if (action == _gamepadBindings.map()->cend()) return;

	invokeCallbacks(KeyEvent{state}, action->second);
}

void EventManager::injectMouse2DAxisInput(const Events::Mouse2DAxis axis, const glm::vec2 position, const glm::vec2 delta) {
	const auto action = _mouse2DAxisBindings.map()->find(axis);
	if (action == _mouse2DAxisBindings.map()->cend()) return;

	invokeCallbacks(AxisEvent<glm::vec2>{position, delta}, action->second);
}

void EventManager::injectMouse1DAxisInput(const Events::Mouse1DAxis axis, const float position, const float delta) {
	const auto action = _mouse1DAxisBindings.map()->find(axis);
	if (action == _mouse1DAxisBindings.map()->cend()) return;

	invokeCallbacks(AxisEvent<float>{position, delta}, action->second);
}

void EventManager::injectGamepad2DAxisInput(const Events::Gamepad2DAxis axis, const glm::vec2 position, const glm::vec2 delta) {
	const auto action = _gamepad2DAxisBindings.map()->find(axis);
	if (action == _gamepad2DAxisBindings.map()->cend()) return;

	invokeCallbacks(AxisEvent<glm::vec2>{position, delta}, action->second);
}

void EventManager::injectGamepad1DAxisInput(const Events::Gamepad1DAxis axis, const double position, const double delta) {
	const auto action = _gamepad1DAxisBindings.map()->find(axis);
	if (action == _gamepad1DAxisBindings.map()->cend()) return;

	invokeCallbacks(AxisEvent<double>{position, delta}, action->second);
}

EventCallback* EventManager::setActionCallback(const std::initializer_list<uint32_t>& actions, EventCallback callback) {
	EventCallback* callAddress = &callback;
	for (const auto &action: actions) {
		(*callAddress)(Event{action, KeyEvent{ Events::kPress }});
		_actionCallbacks.insert(std::make_pair(action, callback));
	}

	return callAddress;
}

void EventManager::removeActionCallback(const uint32_t& action, const EventCallback* callbackAddress) {
	auto range = _actionCallbacks.equal_range(action);
	
	for (auto rIter = range.first; rIter != range.second; rIter++) {
		if (&rIter->second == callbackAddress) {
			_actionCallbacks.erase(rIter);
			return;
		}
	}

	// TODO: not working, need a rewrite (maybe)
}

const uint32_t EventManager::addBinding(const std::string& action, const Key& key) {
	return _keyBindings.insert(std::make_pair(key, kNoModifier), action);
}

const uint32_t EventManager::addBinding(const std::string& action, const Key& key, const uint32_t& modifiers) {
	return _keyBindings.insert(std::make_pair(key, modifiers), action);
}

const uint32_t EventManager::addBinding(const std::string& action, const MouseButton& mouse) {
	return _mouseBindings.insert(mouse, action);
}

const uint32_t EventManager::addBinding(const std::string& action, const GamepadButton& button) {
	return _gamepadBindings.insert(button, action);
}

const uint32_t EventManager::add2DAxisBinding(const std::string& action, const Mouse2DAxis& axis) {
	return _mouse2DAxisBindings.insert(axis, action);
}

const uint32_t EventManager::add1DAxisBinding(const std::string& action, const Mouse1DAxis& axis) {
	return _mouse1DAxisBindings.insert(axis, action);
}

const uint32_t EventManager::add2DAxisBinding(const std::string& action, const Gamepad2DAxis& axis) {
	return _gamepad2DAxisBindings.insert(axis, action);
}

const uint32_t EventManager::add1DAxisBinding(const std::string& action, const Gamepad1DAxis& axis) {
	return _gamepad1DAxisBindings.insert(axis, action);
}

void EventManager::removeBinding(const Key& key) {
	_keyBindings.remove(std::make_pair(key, kNoModifier));
}

void EventManager::removeBinding(const Key& key, const KeyModifier& modifier) {
	_keyBindings.remove(std::make_pair(key, modifier));
}

void EventManager::removeBinding(const MouseButton& button) {
	_mouseBindings.remove(button);
}

void EventManager::removeBinding(const GamepadButton& button) {
	_gamepadBindings.remove(button);
}

void EventManager::remove1DAxisBinding(const Gamepad1DAxis& axis) {
	_gamepad1DAxisBindings.remove(axis);
}

void EventManager::remove1DAxisBinding(const Mouse1DAxis& axis) {
	_mouse1DAxisBindings.remove(axis);
}

void EventManager::remove2DAxisBinding(const Gamepad2DAxis& axes) {
	_gamepad2DAxisBindings.remove(axes);
}

void EventManager::remove2DAxisBinding(const Mouse2DAxis& axes) {
	_mouse2DAxisBindings.remove(axes);
}

std::pair<EventManager::ActionMap<KeyCombination>::constIter, EventManager::ActionMap<KeyCombination>::constIter> EventManager::getKeyBindings() { return _keyBindings.actionBindings(); }

std::pair<EventManager::ActionMap<MouseButton>::constIter, EventManager::ActionMap<MouseButton>::constIter> EventManager::getMouseKeyBindings() { return _mouseBindings.actionBindings(); }

tl::expected<const std::string, Error*> EventManager::getActionName(const uint32_t &actionHash) const {
	if (_keyBindings.nameMapping()->find(actionHash) != _keyBindings.nameMapping()->cend()) {
		return _keyBindings.nameMapping()->at(actionHash);
	}
	if (_mouseBindings.nameMapping()->find(actionHash) != _mouseBindings.nameMapping()->cend()) {
		return _mouseBindings.nameMapping()->at(actionHash);
	}
	if (_mouse1DAxisBindings.nameMapping()->find(actionHash) != _mouse1DAxisBindings.nameMapping()->cend()) {
		return _mouse1DAxisBindings.nameMapping()->at(actionHash);
	}
	if (_mouse2DAxisBindings.nameMapping()->find(actionHash) != _mouse2DAxisBindings.nameMapping()->cend()) {
		return _mouse2DAxisBindings.nameMapping()->at(actionHash);
	}
	if (_gamepadBindings.nameMapping()->find(actionHash) != _gamepadBindings.nameMapping()->cend()) {
		return _gamepadBindings.nameMapping()->at(actionHash);
	}
	if (_gamepad1DAxisBindings.nameMapping()->find(actionHash) != _gamepad1DAxisBindings.nameMapping()->cend()) {
		return _gamepad1DAxisBindings.nameMapping()->at(actionHash);
	}
	if (_gamepad2DAxisBindings.nameMapping()->find(actionHash) != _gamepad2DAxisBindings.nameMapping()->cend()) {
		return _gamepad2DAxisBindings.nameMapping()->at(actionHash);
	}
	return tl::unexpected(new Error(ErrorMessage("Cannot find assigned action with hash {}", actionHash)));
}

}
