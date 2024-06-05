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

#include <stdexcept>
#include <optional>

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include "expected.hpp"
#include "src/error.h"
#include "src/events/eventman.h"
#include "src/platform/gamepadman.h"
#include "src/platform/keyconversion.h"

#include "window.h"

namespace Platform {

Window::Window() {
	//glfwDefaultWindowHints();
	//glfwInit();
	if (!glfwVulkanSupported())
		throw std::runtime_error("Vulkan not supported on this system");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(1280, 1008, "", nullptr, nullptr);

	glfwSetInputMode(_window, GLFW_STICKY_KEYS, GLFW_TRUE);

	glfwSetWindowUserPointer(_window, this);

	glfwSetKeyCallback(_window, &Window::callbackKey);
	glfwSetCursorPosCallback(_window, &Window::callbackMousePosition);
	glfwSetMouseButtonCallback(_window, &Window::callbackMouseButton);
	glfwSetScrollCallback(_window, &Window::callbackMouseScroll);
	glfwSetCursorEnterCallback(_window, &Window::callbackMouseEnter);
	glfwSetFramebufferSizeCallback(_window, &Window::callbackFramebufferSize);
}

Window::~Window() {
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void Window::setTitle(const std::string &title) {
	glfwSetWindowTitle(_window, title.c_str());
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(_window);
}

tl::expected<int, VulkanError*> Window::createVulkanSurface(VkInstance instance, VkSurfaceKHR *surface) {
	VkResult surfaceResult = glfwCreateWindowSurface(instance, _window, nullptr, surface);
	switch (surfaceResult) {
		case VK_SUCCESS: return 0;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			return tl::unexpected(new VulkanError(surfaceResult, ErrorMessage("Vulkan Instance cannot create needed surface")));
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return tl::unexpected(new VulkanError(surfaceResult, ErrorMessage("Window in use by other API. Check window hints")));
		default:
			return tl::unexpected(new VulkanError(surfaceResult, ErrorMessage("Unknown error during surface creation")));
	}
}

void Window::callbackKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
	// fmt::println("{} {}", key, scancode);
	if (action == GLFW_REPEAT)
		return;

	EventMan.injectKeyboardInput(convertGLFW2Key(key), action == GLFW_RELEASE ? Events::kRelease : Events::kPress, mods);

	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_keyCallback)
		return;

	w->_keyCallback(key, scancode, action, mods);
}

void Window::callbackMousePosition(GLFWwindow *window, double xpos, double ypos) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	glm::vec2 absolute = glm::vec2(xpos, ypos);
	glm::vec2 delta = absolute - w->_lastMousePosition.value_or(absolute);

	EventMan.injectMouse2DAxisInput(Events::kMousePosition, absolute, delta);
	
	if (w->_mousePositionCallback)
		w->_mousePositionCallback(absolute, delta);

	w->_lastMousePosition = absolute;
}

void Window::callbackMouseScroll(GLFWwindow *window, double xpos, double ypos) {
	glm::vec2 position(xpos, ypos);

	EventMan.injectMouse1DAxisInput(Events::kMouseScrollHorizontal, position.x, position.x);
	EventMan.injectMouse1DAxisInput(Events::kMouseScrollVertical, position.y, position.y);
	
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_mouseScrollCallback)
		return;

	w->_mouseScrollCallback(position, position);
}

void Window::callbackMouseEnter(GLFWwindow *window, int entered) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	
	if (entered) {
		double x, y;
		glfwGetCursorPos(w->_window, &x, &y);
		w->_lastMousePosition = glm::vec2(x, y);
	} else { // leaving the window
		w->_lastMousePosition = {};
	}

	if (!w->_mouseEnterCallback)
		return;

	w->_mouseEnterCallback(entered);
}

void Window::callbackMouseButton(GLFWwindow *window, int button, int action, int mods) {
	if (action == GLFW_REPEAT)
		return;

	EventMan.injectMouseButtonInput(static_cast<Events::MouseButton>(button), action == GLFW_RELEASE ? Events::kRelease : Events::kPress);

	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	if (!w->_mouseButtonCallback)
		return;

	w->_mouseButtonCallback(button, action, mods);
}

void Window::callbackFramebufferSize(GLFWwindow *window, int width, int height) {
	Window *w = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
}

void Window::setKeyCallback(const KeyCallback &keyCallback) {
	_keyCallback = keyCallback;
}

void Window::setMouseButtonCallback(const MouseButtonCallback &mouseButtonCallback) {
	_mouseButtonCallback = mouseButtonCallback;
}

void Window::setMousePositionCallback(const Axis2DCallback &mousePositionCallback) {
	_mousePositionCallback = mousePositionCallback;
}

void Window::setMouseScrollCallback(const Axis2DCallback &mouseScrollCallback) {
	_mouseScrollCallback = mouseScrollCallback;
}

void Window::setMouseEnterCallback(const MouseEnterCallback &mouseEnterCallback) {
	_mouseEnterCallback = mouseEnterCallback;
}

bool Window::isMouseCursorVisible() {
	return glfwGetInputMode(_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

void Window::setMouseCursorVisible(bool visible) {
	if (visible)
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::setRawMouseMotion(bool enabled) {
	if (!glfwRawMouseMotionSupported()) {
		spdlog::warn("Raw mouse motion is not supported");
		return;
	}
	
	if (enabled)	
		glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	else
		glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

GLFWwindow * Window::getWindowHandle() {
	return _window;
}

#ifdef WITH_VULKAN

const char ** Window::getInstanceExtensions(unsigned int &numExtensions) {
	return glfwGetRequiredInstanceExtensions(&numExtensions);
}

void Window::createWindowSurface(VkInstance &instance, VkSurfaceKHR &surface) {
	VkResult result = glfwCreateWindowSurface(instance, _window, nullptr, &surface);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Error creating window surface");
}

#endif

void Window::getSize(unsigned int &width, unsigned int &height) {
	glfwGetFramebufferSize(_window, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
}

}
