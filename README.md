# COMP465_CG

### Environement
  - Visual Studio (2013 -2017)
  - GLEW
  - GLM
  - freeglut
  
### Implementation
  - This is a simple simulation game. (spaceship VS turrets)
  - It contains a solar system with several planets that rotates around the sun. (Model Transformation (Trans, rotate, scale))
  - There is a spaceship that can rotates by x,y,z axis with quaternion rotation method. (Avoid gimble lock!!)
  and space ship and two missile(rocket) turrets on planets can shoot smart missile (rocket) 
  (using dot and cross product and sign of co-linear vector multiplication) also spaceship can warp to other planets
  - Put one big image near to camera's vanishing point dynamically, so i didnt make a sky box cube for a background
  - Implemented ambient, diffuse(model shade) and spot light(from sun) 
  
### Demo
![](./Demo.gif)
