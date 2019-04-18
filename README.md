# OpenGL Glut 3D Maze
Midterm Project for Graphics Course (CS-357)

This program features a randomly generated mario-themed maze along with a first person controller that can traverse the enviroment. It combines the kno

## Controls
Movement:
  * W - move player forward
  * A - move player left
  * S - move player backward
  * D - move player right
  * Mouse Movement - rotate the camera (look around)
  
Window Actions:
  * ESC - Disable mouse centering and input
  * Q - quit the program


## Collision Detection
Collision is handled by making use of a basic distance function. This effectively generates invisible spheres (colliders) which prevent users from moving inside drawn objects.

![Capture3](https://user-images.githubusercontent.com/32151128/56340897-29f62100-6168-11e9-8b3f-c792c7dfcc21.JPG)

## Maze Generation
 1. An entry point is chosen along the "right" side of the maze
 2. Each adjecent block is tested to be a viable option for being the next path point, a randmon option is chosen to be removed
 3. Step 2 is done until no more options are left, the algorithm pops path points off the stack until an option does exist
 4. Once the stack is completely empty no more paths can be created, an exit point is chosen randomly on the "left" side
 5. A square area in the middle of the maze is cleared for planting the tree object (if maze size permits)
 
 ![Capture2](https://user-images.githubusercontent.com/32151128/56340895-295d8a80-6168-11e9-8c12-93ee3f664661.JPG)
 
 ## Other Components and Features
  * Textured bricks and skyBox
  * Mouse centering
  * Projection, translation and scale matricies usage
 
 ![Capture3](https://user-images.githubusercontent.com/32151128/56340897-29f62100-6168-11e9-8b3f-c792c7dfcc21.JPG)
