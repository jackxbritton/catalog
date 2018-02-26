# Overview.

A very tiny C program for detecting file changes with inotify. Just initialize a Catalog struct, define a callback function, and associate it with a filename with catalog\_add. Then call catalog\_service routinely to check for file changes.

# Building.

I use cmake.

```
mkdir build
cd build
cmake ..
make
```

This builds the example project. The opengl example depends on SDL2 and GLEW.

# Examples.

## basic

This is the simplest possible example. Catalog entries are inserted for files a.txt, b.txt, c.txt, and d.txt. You will need to create these files!

```
touch a.txt b.txt c.txt d.txt
```

When the files are modified, the callback function in basic.c is called with the argument specified in catalog\_add.

## opengl

This is a quick and messy implementation of shader and texture hotloading in OpenGL. Once the program is running, try opening examples/test.png in GIMP and editing it, or try editing the shader files in examples/shaders. The SDL window should update live with the changes.
