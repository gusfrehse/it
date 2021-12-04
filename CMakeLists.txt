cmake_minimum_required(VERSION 3.15)

project(it CXX)

find_package(SDL2 CONFIG REQUIRED)

add_executable(it src/main.cpp)
target_compile_features(it PRIVATE cxx_std_20)

target_link_libraries(it PRIVATE
	SDL2::SDL2
	SDL2::SDL2main
	SDL2::SDL2-static)
