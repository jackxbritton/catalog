# Overview.

Very tiny C program for detecting file changes with inotify. Just initialize a Catalog struct, define a callback function, and associate it with a filename with catalog\_add. Then call catalog\_service routinely to check for file changes.

# Building.

Very standard build process with cmake:

```
mkdir build
cd build
cmake ..
make
```
