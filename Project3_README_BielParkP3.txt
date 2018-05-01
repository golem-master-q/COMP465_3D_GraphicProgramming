===============================================
||		PROJECT 3 README	      ||
===============================================
Team Mebers:
Inkyu Park - inkyu.park.12@my.csun.edu
Dino Biel -  dino.biel.934@my.csun.edu

Project Introduction: Project 3 had us expand upon project 2 by adding lighting and texture. We also improved some features that were missing before from Project 2.

Added (and Fixes in Project 3):
+ Added Background Texture plane for all views (star field)
+ Added Ambient Light
+ Added Point Light 
+ Added Headlight 

!! Important note for Lights and Textures: We elected to not add toggles for lights or texture since they were extra and not required. 

Fixes from Project 2
+ Implimented pass/resign
+ Implimented ship Gravity properly (Gravity is now a constant force on the ship when you turn it on)
+ Added full collision detection. (if you collide with any rigid body you die instantly)


==========================================================================================================================================================
Buttons: The following describes inputs and what they do
==========================================================================================================================================================
t/T:	Handles time quantum. We have the following TQs: 5,40,100,500
v/V:	Changes to next camera
x/X:	Changes to previous camera
g/G:	Toggles gravity on/off
w/W:	Toggles warp between unum/duo (and orients ship to face target)
q/Q:	Quits the program

UP Arrow:	moves ship forward
Down Arrow:	moves ship backward
Left Arrow:	rotates ship left
Right Arrow:	rotates ship right
CTRL + UP:	pitches ship upward
CTRL + DOWN:	pitches ship downard
CTRL + LEFT:	Rolls ship left
CTRL + RIGHT:	Rolls ship right
==========================================================================================================================================================
Ship Movement:
==========================================================================================================================================================
  - Forward: We calculate the ship's move vector by multiplying ship's local -Z axis movement translation
  with ship's rotation matrix. And we make translation matrix with that move vector
  and multiply with old translation matrix to update ship's translation matrix.

  - Backward: We calculate the ship's move vector by multiplying ship's local +Z axis movement translation
  with ship's rotation matrix. And we make translation matrix with that move vector
  and multiply with old translation matrix to update ship's translation matrix.

  - Turn Left/Right: We make a rotation matrix with a rotate radian (Left:+,Right:-) by Y-axis
  and multiply with old ship's rotation matrix to update rotation matrix.

  - Pitch Up/Down: We make a rotation matrix with a rotate radian (Up:+,Down:-) by X-axis
  and multiply with old ship's rotation matrix to update rotation matrix.

  - Roll Left/Right: We make a rotation matrix with a rotate radian (Left:+,Right:-) by X-axis
  and multiply with old ship's rotation matrix to update rotation matrix.

==========================================================================================================================================================
Warp:
==========================================================================================================================================================
  - The ship can warp to unum and duo by pressing 'w' button
  - When we warp the ship, we calculate the target vector by planet's position minus planet's camera position and normalize it.
    And we always calculate the rotate radian by dot product of the target vector and -Z(0, 0, -1) vector (arc cosine also applied).
    then we always use +Y axis to rotate the ship and we translate the ship to camera position.

==========================================================================================================================================================
Missiles (Rockets) (ship, turrets):
==========================================================================================================================================================
  - Ship Missile(Rocket): when 'f' button pressed, the ship rocket become active and the rocket's time frame starts to count and it only has 9 missiles

    1) The first 200 time frames of the missile:
      the missile only move forward without tracking the target turret (rocket move vector based on ship's At vector)
    2) After 200 and less than 2000 time frames of the missile:
      Missile finds the target by calculating the distance between the current rocket position and target turret position,
      and it follows the target. (after the target set up, the target is not changed until the rocket become inactive)

  - Unum&Duo Turret Missile(Rocket): it calculates the distance between the ship position and turret position every time in update function
  and when it is less than the certain range, the turret shoots smart missile and starts to count the rocket's time frame,
  and the missile follows the ship until it hits the ship or the time frame of the rocket ends. Each Turret can shoot 5 missles max.

  - Smart Missile(Rocket) function: This function calculates the missile's rotation radian with target vector and rocket's At vector.
  It uses dot product to get a rotate radian and cross product for an axis of rotation, to achieve the correct rotation quaternion,
  it uses cross(At, target), and we multiply quaternion vector to rocket's old Right, Up and At to update them.
  Then, it replaces rotation matrix's Right, Up and At vectors with the updated Right, Up and At vectors.
  Lastly, it translates the missile with new At vector.
==========================================================================================================================================================
Gravity:
==========================================================================================================================================================
- Gravity is applied by pressing 'g'
- Gravity is now a constant and is always being applied to the ship constantly.

==========================================================================================================================================================
Lighting
==========================================================================================================================================================
- We do most of the calculations in the .cpp file (in the display function) and then pass it to the appropriate shaders
- The position for all the lighting vectors was placed in the ModelView space (view * model) to get the correct world position for the verticies
- The lighting is primarily handled in the fragment shader.
- Ambient lighting is simply a float value that can be altered in the fragment shader
- The point light is also supplied in the fragment shader ( Point Light : light_source - vertex_worldPosition )
- The headlight is also supplied in the fragment shader ( Directional light : vec3(0.0f, 0.0f, 1.0f) )


==========================================================================================================================================================
Texture
==========================================================================================================================================================
- We make one big plane that can cover the backside of the frustrum so we can texture it and make it as a background.
- We translate and rotate the textured plane to the vanishing point for each views(little closer than actual vanishing point), so we can always have background.
- We made a boolean variable that tells shader to handle differently for texture.
- Vertex shader get all the world position of plane's vertex and in fragment shader it actually put 2D image (star.raw) on the plane. 
- 