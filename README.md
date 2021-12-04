# it
## Compiling
### Linux
In the shell run:

	git clone --recursive <this-repo>
    sudo apt install libx11-dev libxft-dev libxext-dev
	mkdir build
	./vcpkg/bootstrap-vcpkg.sh
	./vcpkg/vcpkg install sdl2[x11]:x64-linux
	cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake

The executable will be at ```./build/it```

