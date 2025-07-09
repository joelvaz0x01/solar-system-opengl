# Solar system made with OpenGL
This project is a simple solar system simulation using OpenGL. It features the Sun, and planets with their own textures.


## 📦 Requirements
- C++ compiler (C++11 or later)
- OpenGL development libraries
- GLFW library for window management
- GLM library for mathematics


## 🛠️ Installation

### Prerequisites
- C++ compiler (GCC, Clang, MSVC)
- CMake (version 3.5 or later)
- OpenGL libraries (only for Linux and macOS)
- Graphic Interface

All other dependencies like GLFW, GLM, GLAD, and FreeType will be built automatically using CMake.

### Installation Steps
The installation steps below worked on Windows.

If CMake won't work on Linux and macOS, probably you will need to install some dependencies manually and/or fix the `CMakeLists.txt` file.

```bash
# Clone repository
git clone --recursive https://github.com/yourusername/solar-system-opengl.git

# Change to project directory
cd solar-system-opengl
mkdir build

# Build project
cmake -B build -DCMAKE_BUILD_TYPE=Release -S .
cmake --build build --config Release

# Run application
cd ./bin/Release
./solar_system.out.exe
```

## 🎮 Controls
### Movement Controls
| Key | Action |
|-----|--------|
| `W` `A` `S` `D` | Move camera forward/left/backward/right |
| `Q` | Move camera down |
| `E` | Move camera up |
| `Mouse` | Look around |
| `Mouse Wheel` | Zoom in and out |
| `ESC` | Close the window |

### Camera Modes
| Key | Mode |
|-----|------|
| `SPACE` | Free camera mode (default) |
| `0` | Top view camera mode |
| `1` - `8` | Focus on planets (Mercury to Neptune) |
| `Numpad 1-8` | Alternative planet focus keys |

### Skybox Modes
| Key | Skybox |
|-----|--------|
| `F1` | Purple nebula complex skybox (default) |
| `F2` | Green nebula skybox |


## 🤝 Contributors
- [Bruno Gonçalves](https://github.com/BrunoFG1)
- [Joel Vaz](https://github.com/joelvaz0x01)
