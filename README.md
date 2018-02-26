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

# Why?

I want to hotload assets when I write tiny games. I may end up using this code for other stuff, but that's the motivation right now.
