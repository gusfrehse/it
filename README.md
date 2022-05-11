# it
# Compiling
For both os' you will need CMake. You can change the generator
(Makefile, Ninja, Visual Studio, etc..) in the first call to CMake,
passing the `-G <generator>` argument.

## Specially for Linux
Comment the `set(VCPKG_TARGET_TRIPLET x64-windows-static)`, uncomment
the `set(VCPKG_TARGET_TRIPLET x64-windows-static)` and comment the
`WIN32`, all in the `CMakeLists.txt` file.

Then, in the shell, run:
    
    sudo apt install libx11-dev libxft-dev libxext-dev # or the equivalent for your package manager

## Windows and Linux
You will need Visual Studio (or the `cl.exe` compiler) for windows and a C compiler for linux.

Run:

    git clone --recursive <this-repo>
    mkdir build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build

The executable will be somewhere inside the `build` directory.
