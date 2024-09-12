# renderer

This is a renderer for the .obj file format of 3D models often used in 
applications like blender. Aside from the use of SDL to create a window and 
draw 2D points all code has been created by myself. This includes:
- .obj parser
- vector and matrix implementations
- perspective rendering
- lighting reflections

## Using the program:

To run the program simply run `main` with an optional argument of a supplied 
.obj file to render.

## Building

As this program requires the use of SDL users must install the required packages
in order to build. For instructions on installing the dependencies look at the 
(SDL wiki)[https://wiki.libsdl.org/SDL2/Installation].
