# Imagine: 3D Environment Engine v. 0.1.0
---
The Imagine Engine's fundamental purpose is to be a platform from which to teach myself the basic technical concepts of 3D rendering and software project management.

### The project has two goals:

1. Develop an open source 3D environment engine that includes enough stable, general, easy to use, features to be useful to the public (with an emphasis on the Linux community).

2. To use the engine to develop a complete video game that passes the "Hey, come take a look at this! I made that!" quality test and good enough to be accepted on a video game marketplace such as Steam or Humble Bundle.

### Design Philosophies:

* Just Make it Work: Good planning is, of course, key to good software, but over-planning can be just as crippling as no planning at all. Therefore, when implementing new components I will (after reasonable planning) follow the "just make it work" philosophy. 
* Evolve Over Time: There is no possible way for me to write the perfect code the first time (especially while I'm learning). Therefore, it is okay to write imperfect but functional code, at first, and re-factor periodically to reflect the evolution of my skill and knowledge. I always strive for improvement, but realize the idea of perfection is a mental prison.
* Nothing is Set in Stone: Everything is changeable. I don't have to worry about choosing the wrong direction or implementation. If I don't like it, if it isn't good enough, or if it doesn't work, no problem! It can be changed any way I see fit.

Note: This project is a primarily a learning tool developed for fun, in my free time, and is not meant to compete or compare to other software. That being said, I try my best to make valuable things for people to use.

---
### Getting things to run
The project is still in development and will be for some time. However, if you'd like to try to build the project and checkout the progress, here is some information that might be helpful.

Currently, the project has been successfully built on the following systems...

1. Linux Mint 18.1 Serena 64-bit | nVIDIA GeForce GTX 970/PCIe/SSE2 | 4.5.0 NVIDIA 375.66
2. Linux Mint 18.1 Serena 64-bit | Mesa DRI Intel HD Graphics 620 (Kabylake GT2) | 4.5 (Core Profile) Mesa 17.0.7

In the future, the goal is to make the project Linux/Windows/Mac compatible.

#### Controls
**This is somewhat important!** By default the engine hides and locks the mouse into the window for camera use, even if nothing renders to the screen. **To get your mouse back simply press "e".** You can also press "Esc" to quit at any time. By default, move with WASD and look around with the mouse. Movement can only take place in "grab mode" when the program has control of the mouse. To toggle grab mode press "e". To toggle First Person/Third Person mode press "c".

#### Dependencies:

C++ Version: C++11   
GLSL Version: 330 core (or above)

OpenGL 3.3 core (or above)  
https://www.opengl.org/

OpenGL Mathematics v. 0.9.7.2-1 or higher  
https://glm.g-truc.net/0.9.8/index.html

Simple DirectMedia Layer 2.0  
https://www.libsdl.org/

SDL_image 2.0  
https://www.libsdl.org/projects/SDL_image/

#### Makefile

To compile: make compile  
To compile and run: make compile test

---
### Contact and Collaboration

Feel free to contact me at davidlipps.github@gmail.com

I welcome constructive comments, advice and banter. For now, I will remain the sole contributor to this repo; however, after a solid foundation is laid and I gain enough experience, that may change. Feel free to make your own copy and do your thing... show me what you got... 

---
### Version

The current version is v. 0.1.0 to denote initial development. If/When I decide the project has evolved enough, I may increment the minor version value as I see fit.

---
### License

The Imagine: 3D Environment Engine v 0.1.0 is currently licensed under the MIT License.   
Copyright © 2017 David E Lipps

Credit is appreciated.