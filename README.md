# Zaboard

### Building on Windows
```bash
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ../

# Open the visual studio solution file in the build folder and build using Visual Studio
```

### Building on Linux
```bash
mkdir build && cd build
cmake ../
make
```

### Running Clang-Format
```bash
cd src
clang-format -i ./**/*.*
```