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

#ifndef OPENAWE_EVENTMAN_H
#define OPENAWE_EVENTMAN_H

#include <cstdint>
#include <map>
#include <optional>
#include <bitset>
#include <functional>

#include "src/singleton.h"
#include "src/crc32.h"
#include "src/error.h"

#include "src/events/key.h"
#include "src/events/mouse.h"
#include "src/events/gamepad.h"
#include "src/events/event.h"

namespace Events {

typedef std::function<void(const Event &event)> EventCallback;
typedef std::pair<Key, uint32_t> KeyCombination;

/*!
 * \brief Class for exchanging input
 *
 * This class is a singleton for exchanging inputs to callbacks. A real input is for example a key on the keyboard or a
 * certain axis for example the mouse position, the scroll wheel or the game controller thumb stick. These raw input
 * events are mapped to one or more abstract actions. A callback is associated with multiple actions. If an input is
 * injected using the inject* methods, the assodciated actions are retrieved and the event is passed to the callbacks
 * associated actions.
 *
 * This design was chosen to decouple raw input events from the actions and allow to change the keybindings to be
 * changed at runtime.
 */
class EventManager : public Singleton<EventManager> {
public:
	template<typename T>
	class ActionMap {
	public:
		typedef std::map<uint32_t, std::string> nameMap;
		typedef nameMap::const_iterator nameMapConstIter;
		typedef std::map<T, uint32_t> actionMap;
		typedef typename actionMap::const_iterator constIter;

		bool hasAction(const uint32_t &actionHash) const { return _actionNames.find(actionHash) != _actionNames.end(); };
		const std::string actionNameFromHash(const uint32_t &actionHash) const { return _actionNames.at(actionHash); };

		uint32_t insert(const T &key, const std::string &action) { 
			const uint32_t actionHash = Common::crc32(action);
			if (hasAction(actionHash)) return 0;

			_map.insert(std::make_pair(key, actionHash)); 
			_actionNames.insert(std::make_pair(actionHash, action));

			return actionHash;
		};

		void remove(const T &key) {
			auto action = _map.find(key);
			_map.erase(action);
		};
		
		std::pair<nameMapConstIter, nameMapConstIter> actionNames() { return std::make_pair(_actionNames.cbegin(), _actionNames.cend()); }; 
		std::pair<constIter, constIter> actionBindings() { return std::make_pair(_map.cbegin(), _map.cend()); }; 

		const actionMap* map() const { return &_map; };
		const nameMap* nameMapping() const { return &_actionNames; };
	private:
		actionMap _map;
		nameMap _actionNames;
	};

	/*!
	 * Inject a keyboard event to the event system
	 * \param key The key for the event
	 * \param state If the key is pressed or released
	 */
	void injectKeyboardInput(const Key key, const KeyState state, const uint32_t modifiers);

	/*!
	 * Inject a mouse button event to the event system
	 * \param key The key for the event
	 * \param state If the key is pressed or released
	 */
	void injectMouseButtonInput(const MouseButton mouse, const KeyState state);

	/*!
	 * Inject a mouse 2D axis event to the event system
	 * \param position Current mouse 2D axis values 
	 * \param delta Change from previous mouse 2D axis values
	 */
	void injectMouse2DAxisInput(const Mouse2DAxis axis, const glm::vec2 position, const glm::vec2 delta);

	/*!
	 * Inject a mouse 1D axis event to the event system
	 * \param position Current mouse 1D axis values 
	 * \param delta Change from previous mouse 1D axis values
	 */
	void injectMouse1DAxisInput(const Mouse1DAxis axis, const float position, const float delta);

	/*!
	 * Inject a gamepad button event to the event system
	 * \param key The key for the event
	 * \param state If the key is pressed or released
	 */
	void injectGamepadButtonInput(const GamepadButton button, const KeyState state);

	/*!
	 * Inject a gamepad 2D axis event to the event system
	 * \param position Current gamepad 2D axis values 
	 * \param delta Change from previous gamepad 2D axis values
	 */
	void injectGamepad2DAxisInput(const Gamepad2DAxis axis, const glm::vec2 position, const glm::vec2 delta);

	/*!
	 * Inject a gamepad 1D axis event to the event system
	 * \param position Current gamepad 1D axis values 
	 * \param delta Change from previous gamepad 1D axis values
	 */
	void injectGamepad1DAxisInput(const Gamepad1DAxis axis, const double position, const double delta);

	/*!
	 * Set a callback for a set of actions
	 * \param actions The actions, the callback is associated with
	 * \param callback The callback, to be associated with actions
	 */
	EventCallback* setActionCallback(const std::initializer_list<uint32_t>& actions, EventCallback callback);
	//EventCallback* setActionCallback(const std::initializer_list<uint32_t>& actions, EventCallback callback);

	/*!
	 * Remove a callback for an action
	 * \param action The action, the callback is associated with
	 * \param callback The callback, to be associated with actions
	 */
	void removeActionCallback(const uint32_t& action, const EventCallback* callbackAddress);

	/*!
	 * Associate an action with a specific keyboard key
	 * \param action The action for association with the key
	 * \param key The key for association with the action
	 */
	const uint32_t addBinding(const std::string& action, const Key& key);

	/*!
	 * Associate an action with a specific keyboard key combination
	 * \param action The action for association with the key
	 * \param key The key for association with the action
	 */
	const uint32_t addBinding(const std::string& action, const Key& key, const uint32_t& modifiers);

	/*!
	 * Associate an action with a specific mouse key
	 * \param action The action for association with the key
	 * \param key The key for association with the action
	 */
	const uint32_t addBinding(const std::string& action, const MouseButton& mouse);

	/*!
	 * Associate an action with a specific gamepad buttpn
	 * \param action The action for association with the button
	 * \param key The button for association with the action
	 */
	const uint32_t addBinding(const std::string& action, const GamepadButton& button);

	/*!
	 * Associate an action with 2D axis mouse movement
	 * \param action The action for association
	 * \param axis The axis for association with the action
	 */
	const uint32_t add2DAxisBinding(const std::string& action, const Mouse2DAxis& axis);

	/*!
	 * Associate an action with 1D axis mouse movement
	 * \param action The action for association
	 * \param axis The axis for association with the action
	 */
	const uint32_t add1DAxisBinding(const std::string& action, const Mouse1DAxis& axis);

	/*!
	 * Associate an action with 2D axis gamepad stick movement
	 * \param action The action for association
	 * \param axis The axis for association with the action
	 */
	const uint32_t add2DAxisBinding(const std::string& action, const Gamepad2DAxis& axis);

	/*!
	 * Associate an action with 1D gamepad stick/trigger movement
	 * \param action The action for association
	 * \param axis The axis for association with the action
	 */
	const uint32_t add1DAxisBinding(const std::string& action, const Gamepad1DAxis& axis);

	void removeBinding(const Key& key);
	void removeBinding(const Key& key, const KeyModifier& modifiers);
	void removeBinding(const MouseButton& button);
	void removeBinding(const GamepadButton& button);
	void remove1DAxisBinding(const Gamepad1DAxis& axis);
	void remove1DAxisBinding(const Mouse1DAxis& axis);
	void remove2DAxisBinding(const Gamepad2DAxis& axes);
	void remove2DAxisBinding(const Mouse2DAxis& axes);

	std::pair<ActionMap<KeyCombination>::constIter, ActionMap<KeyCombination>::constIter> getKeyBindings();

	std::pair<ActionMap<MouseButton>::constIter, ActionMap<MouseButton>::constIter> getMouseKeyBindings();

	tl::expected<const std::string, Error*> getActionName(const uint32_t &actionHash) const;

private:
	std::multimap<uint32_t, EventCallback> _actionCallbacks;

	ActionMap<KeyCombination> _keyBindings;
	ActionMap<MouseButton> _mouseBindings;
	ActionMap<GamepadButton> _gamepadBindings;
	ActionMap<Mouse2DAxis> _mouse2DAxisBindings;
	ActionMap<Mouse1DAxis> _mouse1DAxisBindings;
	ActionMap<Gamepad2DAxis> _gamepad2DAxisBindings;
	ActionMap<Gamepad1DAxis> _gamepad1DAxisBindings;

	inline void invokeCallbacks(EventData data, uint32_t action);
};

} // End of namespace Events

#define EventMan Events::EventManager::instance()

#endif //OPENAWE_EVENTMAN_H
