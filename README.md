## 🌲 feu2forêt - Forest Fire Simulator

This C project simulates the propagation of a forest fire using a cellular automaton. The initial state of the forest is generated from a BMP image: the program combines the luminance of the image's pixels with a 50% probability density to place the trees.

**Main Features**
* **Image-Based Generation**: The forest is modeled from a BMP image read via the SDL2 library. Pixels with high luminance have a higher chance of spawning as trees.
* **Cellular Automaton**: The simulation uses four states: Empty, Tree, Fire, and Calcined (ashes).
* **8-Connected Propagation**: Once ignited on the top and bottom rows, the fire spreads to the 8 adjacent cells (horizontally, vertically, and diagonally).
* **Interactive Control**: Pressing the **Space** key allows you to pause or resume the simulation.
* **Classic Version**: The `justefeu2foret.c` file offers an alternative, simplified simulator with a purely random initial density (65%) and strict 4-connected propagation, without relying on an input image.

**Prerequisites**
* GCC Compiler
* **GL4Dummies** library
* **SDL2** library
* **OpenGL**

**Compilation**
The included `Makefile` handles all the configuration. Simply run the following command at the root of the project:

> make

*(You can also use `make clean` to clean the workspace or `make zip` to package the project.)*

**Usage**
Run the compiled program by providing a `.bmp` file as a mandatory argument:

> ./feu2foret path/to/image.bmp

*Note: If you want to test the simulation with the included `daft.jpg` file, make sure to convert it to BMP format first, as the code explicitly uses the `SDL_LoadBMP` function.*

**File Architecture**
* `window.c`: Main file managing the window, image loading, and the 8-connected logic of the automaton.
* `justefeu2foret.c`: Simplified, purely random version of the simulation.
* `tiny4D.h`: Utility library containing optimized *static inline* functions for RGBA color manipulation, luminance calculation, and pixel access.
* `Makefile`: Definition of compilation rules and linking options for macOS, Linux, and Windows.
