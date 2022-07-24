cmake_minimum_required(VERSION 3.15)

# Windows
#set(VCPKG_TARGET_TRIPLET x64-windows-static)
# Linux
set(VCPKG_TARGET_TRIPLET x64-linux)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

project(it CXX)

find_package(SDL2 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(it
    #    WIN32
	src/main.cpp
	src/maze.cpp
	src/input_controller.cpp
	src/shaders.cpp
	src/minimap.cpp)

target_compile_features(it PRIVATE cxx_std_20)

target_link_libraries(it PRIVATE
	SDL2::SDL2
	SDL2::SDL2main
	GLEW::GLEW
	glm::glm)
