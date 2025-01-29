# Animation Project

Animation Project demonstrates various techniques related to rendering computer animations using skinned mesh rendering using Windows 10, OpenGL 3.3, and MSVC C++ 11.

Animation Project was created out of a desire to learn more about how the algorithms and data structures used in computer animation.

# Background

Animation Project was created by following the book **Hands-On C++ Game Animation Programming** first edition by _Gabor Szauer_.
The code is functionally equivalent to the book code hosted on the [_PacktPublishing_ repository](https://github.com/PacktPublishing/Hands-On-Game-Animation-Programming) but has been written by myself.

# Usage

Animation Project includes a Visual Studio Solution file that is used to build the project from source. It contains several demos that demonstrate a different technique related to skinned mesh rendering. 

_NOTE:_ The solution file includes a project called _AnimationTests_ that was used for unit testing the math functions. _AnimationTests_ is not included in this repository and can be safely ignored.

## Asset files

After building the project you need to acquire the **asset** files and paste them into `./resource/assets/*`. The asset files contain the animation data and textures.
The assets are included as a part of _Gabor Szauer_'s original repository published by _PacktPublishing_ available [here](https://github.com/PacktPublishing/Hands-On-Game-Animation-Programming/tree/master/Chapter15/Sample01/Assets), [here](https://github.com/PacktPublishing/Hands-On-Game-Animation-Programming/tree/master/Chapter14/Sample01/Assets), [and here](https://github.com/PacktPublishing/Hands-On-Game-Animation-Programming/tree/master/Chapter13/Sample03/Assets)

## Asset Attribution

The 3D model and animations of the character are originally from [quaternius](https://quaternius.com/index.html), available [here](https://quaternius.com/packs/animatedwoman.html).

## Running the Program

After the program is built, it is run with a command line argument specifying which demonstration to use.
The demonstration list is:
- **-animated_model** cycles through the available animations
- **-animation_adding** combines a lean animation with the walk cycle animation
- **-animation_blending** smoothly blends between a walk cycle and run cycle animation
- **-cross_fade_animation** smoothly fades between each of the available animations, randomly selecting animations to change between.
- **-draw_quad** renders a quad, used to verify that OpenGL is working as expected
- **-dual_quaternion_skinning** demonstrates dual quaternion skinning and its differences with linear blended skinning (LBS)
- **-inverse_kinematics** demonstrates a FABRIK (blue) and CCD (red) inverse kinematic solver to move a chain of joints to reach a target location (green) as the target location moves around
- **-skeleton_wireframe** draws the character skeleton, cycles through the available animations
- **-inverse_kinematic_walking** uses inverse kinematics to place the characters feet on an uneven surface, character walks around a simple course
  - this demo can be customized in the file `Animations/demos/WalkingDemo.cpp` in the `WalkingDemo::Initialize()` function by altering the flags in the demoOptions and options struct.

If the demo is not displaying correctly, check that all the files the demo is searching for, listed in the console, are present in the file system. 

# License

Animation Project uses the same license as the code supplied with the **Hands-On C++ Game Animation Programming** book: The MIT License
