# Submarine

Submarine was my final project for an introductory OpenGL course. The project includes a simple interactive scene where you can control a submarine and swim alongside fish.

This project was built on top of the Common Framework for Computer Graphics Courses at FI MUNI, which means it doesn’t include all the source files necessary for a full build but only the source files relevant to this project that I wrote myself. However, you can check out images and video from the project in my [portfolio](https://www.behance.net/gallery/211305315/OpenGL-School-Projects), or download a working build from [itch.io](https://kubak-cz.itch.io/submarine).

**Description:**
The project demonstrates basic knowledge of OpenGL and shaders. The scene contains textured objects, hundreds of (in-shader) animated fish, multiple light sources, custom underwater light attenuation, and a fog effect. The scene is made more dynamic by fish controlled through a flocking algorithm, giving them realistic movement. The submarine features a simple physics simulation with different acceleration and drag on each movement axis.

**Source files:**
- **application.\***: Main application class controlling the whole scene.
- **scripts/**
  - **flock.\***: Class controlling movement and rendering of fish.
  - **floor.\***: Class controlling rendering of infinite floor/ocean surface.
  - **helper.\***: Class with helper functions (random numbers, vector operations).
  - **light_manager.\***: Class for managing multiple light sources.
  - **loader.\***: Class for easy access to loaded assets.
  - **sub.\***: Class for simulating submarine movement and rendering.
  - **uniform_structs.hpp**: Struct definitions for data sent to shaders.
- **shaders/**
  - **fish.\***: Shaders for rendering instanced fish. The vertex shader creates a simple fish animation, and the fragment shader handles underwater light attenuation.
  - **main.\***: Shaders for rendering standard geometry. The fragment shader includes underwater light attenuation.
  - **postprocess.\***: Postprocessing shaders to create the fog effect.
  - **surface.frag**: Fragment shader without light attenuation.
