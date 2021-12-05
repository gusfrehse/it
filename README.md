# it
# Compiling
## Linux
In the shell run:

	git clone --recursive <this-repo>
    sudo apt install libx11-dev libxft-dev libxext-dev # or the your package manager
	./vcpkg/bootstrap-vcpkg.sh
	./vcpkg/vcpkg install sdl2[x11]:x64-linux
	mkdir build
	cmake -S . -B build
	cmake --build build

The executable will be at ```./build/it```

