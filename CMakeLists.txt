cmake_minimum_required (VERSION 3.8)

project ("cgengine")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -fuse-ld=mold")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)

add_subdirectory(third_party)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

# VS only, disabled for now
# set_property(TARGET vulkan_guide PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:vulkan_guide>)

# Add source to this project's executable.
file(GLOB ENGINE_FILES src/*.cpp src/**/*.cpp src/**/**/*.cpp)
file(GLOB ENGINE_HEADERS src/*.h src/**/*.h src/**/**/*.h) 

add_library(engine_src
	${ENGINE_FILES}
	${ENGINE_HEADERS}
)

target_link_libraries(engine_src
      Vulkan::Vulkan 
      vkbootstrap 
      vma 
      glfw 
      glm 
      tinyobjloader 
      imgui 
      stb_image
      expected
      fmt::fmt
      Jolt
      EnTT::EnTT
 )

 target_include_directories(engine_src PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
 target_include_directories(engine_src PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
 target_include_directories(engine_src PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
 target_include_directories(engine_src PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

function(ADD_VK_PROJECT project_folder)
  file(GLOB PROJECT_FILES projects/${project_folder}/**/*.cpp)
  file(GLOB PROJECT_HEADERS projects/${project_folder}/**/*.h) 
  add_executable (${project_folder} 
    projects/${project_folder}/main.cpp
    ${PROJECT_FILES}
    ${PROJECT_HEADERS}
    )

  target_link_libraries(${project_folder}
      engine_src
    )
endfunction()

add_vk_project(pong)
add_vk_project(planets)
add_vk_project(katamari)

find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  message(STATUS "BUILDING SHADER")
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/bin/${FILE_NAME}.spv")
  message(STATUS ${GLSL})
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )
