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

#include "src/events/eventman.h"

#include "eventlistener.h"

namespace Events {

void EventListener::bindCallbacks() {
    for (auto callback = _listenerCallbacks.begin(); callback != _listenerCallbacks.end(); callback++)
        EventMan.setActionCallback({ callback->first }, *(callback->second));
    _isListening = true;
}

void EventListener::addActionCallback(const std::initializer_list<uint32_t>& actions, EventCallback& callback) {
	EventCallback* callAddress = &callback;
	for (const auto &action: actions) {
		_listenerCallbacks.insert(std::make_pair(action, callAddress));
	}
}

void EventListener::unbindCallbacks() {
    for (auto callback = _listenerCallbacks.begin(); callback != _listenerCallbacks.end(); callback++)
        EventMan.removeActionCallback(callback->first, callback->second);
    _isListening = false;
}

bool EventListener::isListening() { return _isListening; }

} // End of namespace Events
