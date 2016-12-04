# Computer Graphics Final Project

### Tyler Lugger

### This Project creates a scene in two different modes that allows navigation in first person view around a natural scene to find a ghost.

#### The scene opens in a forested area in day mode. A ghost is placed randomly within the world, either out in the open or partially within a tree (it is a ghost after all). Use the arrow keys to navigate around the world and find the ghost. When you are close enough to the ghost, a message will display at the top left of the window telling you that you won the hunt. Use the 'd' key to switch to night mode for a more challenging ghost hunt. Use the 'c' key to move the camera directly to the same position of the ghost.

#### What Remains: I still need to fix my trees to make them look more natural. I would also like to add natural looking bushes to add to the scene.

#### The two unique modes for this scene are:
* Day Mode: Navigating through the scene with full light and a moving sun.
* Night Mode: Navigating through the scene with minimal ambient light and a flashlight shining directly in front of the view

### How to run
OpenGL can be installed by following the information on [this webpage](http://www.prinmath.com/csci5229/misc/install.html)


######Complile and run:
* `make`
* `./graphicsProject`

###### Key bindings:
*  o          Open/Close cabin door
*  d          Toggles day/night mode
*  arrows     look left/right, move forward/backward
*  w          Increases camera view angle
*  s          Decreases camera view angle
*  c          Move camera directly to the ghost (cheat to win)
*  f          Toggles FPV/perspective views
*  l          Toggles lighting
*  a/A        Decrease/increase ambient light
*  F1         Toggle smooth/flat shading
*  F2         Toggle local viewer mode
*  F3         Toggle light distance (1/5)
*  F8         Change ball increment
*  m          Toggles light movement
*  []         Lower/rise light
*  x          Toggle axes
*  PgDn/PgUp  Zoom in and out
*  ESC        Exit
