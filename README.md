
# OpenGL_LHC

### Pre-requirement
The pre-requirements are
* OpenGL, GLEW and GLFW
* Freetype
* Pythia 8
* `cmake`

OpenGL is generally pre-installed in macOS so one just need to install the xcode commandline tool:
```
xcode-select --install
```
The `Freetype` and `GLFW` on macOS can be installed by `homebrew`
```
brew install glfw3 freetype
```

### Compilation

`clang` will be used to compile the executable. Before compilation, the following environment variables are needed to be set correctly before the compilation:
```sh
export FREETYPE_DIR="the_path_to_the_Freetype"
export PYTHIA8="the_path_to_the_Pythia8"
```

Go to the root folder of this repository and do:
```sh
mkdir bin
cd bin
cmake ..
```
make sure that `cmake` complete successfully and then
```
make
```

### Note
The relative path is used for shader, make sure it is correctly setup.

