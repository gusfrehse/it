# it
# Compiling
For both os' you will need CMake. You can change the generator (Makefile, Ninja, Visual Studio, etc..) in the first call to CMake, passing the ```-G <generator>``` argument.

## Linux
In the shell run:

	git clone --recursive <this-repo>
    sudo apt install libx11-dev libxft-dev libxext-dev # or the equivalent for your package manager
	./vcpkg/bootstrap-vcpkg.sh
	./vcpkg/vcpkg install sdl2[x11]:x64-linux
	mkdir build
	cmake -S . -B build
	cmake --build build

The executable will be at ```./build/it```

## Windows
You will need Visual Studio (or the ```cl.exe``` compiler).
In the cmd run:

	git clone --recursive <this-repo>
	./vcpkg/bootstrap-vcpkg.sh
	./vcpkg/vcpkg install sdl2:x64-windows
	mkdir build
	cmake -S . -B build
	cmake --build build

The executable will be somewhere inside ```.\build\```, depends where Visual Studio put it.
