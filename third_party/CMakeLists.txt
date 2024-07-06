cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

include(FetchContent)

#add_subdirectory(entt)
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE entt)
target_link_libraries(entt INTERFACE EnTT::EnTT)

add_library(vkbootstrap STATIC)
add_library(glm INTERFACE)
add_library(vma INTERFACE)
add_library(JoltPH INTERFACE)

add_library(stb_image INTERFACE)

add_library(tinyobjloader STATIC)

add_library(expected INTERFACE)
target_sources(expected PRIVATE
  expected/expected.hpp
  )
target_include_directories(expected INTERFACE expected)

target_sources(vkbootstrap PRIVATE 
  vkbootstrap/VkBootstrap.h
  vkbootstrap/VkBootstrap.cpp
  )

target_include_directories(vkbootstrap PUBLIC vkbootstrap)
target_link_libraries(vkbootstrap PUBLIC Vulkan::Vulkan $<$<BOOL:UNIX>:${CMAKE_DL_LIBS}>)

#both vma and glm and header only libs so we only need the include path
target_include_directories(vma INTERFACE vma)

target_include_directories(glm INTERFACE glm)

target_sources(tinyobjloader PRIVATE 
  tinyobjloader/tiny_obj_loader.h
  tinyobjloader/tiny_obj_loader.cc
  )

target_include_directories(tinyobjloader PUBLIC tinyobjloader)

target_link_libraries(glfw)

add_library(imgui STATIC)

target_include_directories(imgui PUBLIC imgui)

target_sources(imgui PRIVATE 
  imgui/imgui.h
  imgui/imgui.cpp

  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp

  imgui/imgui_impl_vulkan.cpp
  imgui/imgui_impl_glfw.cpp
  )

target_link_libraries(imgui PUBLIC Vulkan::Vulkan)

target_include_directories(stb_image INTERFACE stb_image)


# include Jolt
FetchContent_Declare(
  JoltPhysics
  GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
  GIT_TAG "v5.0.0"
  SOURCE_SUBDIR "Build"
)
FetchContent_MakeAvailable(JoltPhysics)
target_include_directories(JoltPH INTERFACE ${JoltPhysics_SOURCE_DIR}/..)

