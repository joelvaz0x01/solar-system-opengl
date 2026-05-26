# Solar system made with OpenGL
This project is a simple solar system simulation using OpenGL. It features the Sun, and planets with their own textures.


## 📦 Requirements
- C++ compiler (with C++17 support)
- OpenGL development libraries
- CMake (version 3.20 or later)
- Graphic Interface for GUI


## 🛠️ Installation and Setup
> [!NOTE]
> macOS was only tested with GitHub Actions, so it may not work on your local machine.

Dependencies like GLFW, GLM, GLAD, and FreeType will be built automatically when running CMake.

To build the project, make sure you have the required dependencies installed on your system.

```bash
# Clone repository
git clone --recursive https://github.com/joelvaz0x01/solar-system-opengl.git

# Change to project directory
cd solar-system-opengl
mkdir build

# Build project
cmake -B build -DCMAKE_BUILD_TYPE=Release -S .
cmake --build build --config Release
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
