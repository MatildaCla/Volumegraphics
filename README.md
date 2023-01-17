# Volumegraphics

A group project with Roger Viñeta I Mañe based on a framework by Javi Agenjo 2013. 

The main focus was to write a shader that displays volumes with a ray marching algorithm. At first we worked on loading the volume files into the framework. Then we implemented the shader that contains the ray marching algorithm and other features like transfer functions and jittering. All features are implemented using IMGui and can be adjusted at runtime through the UI window.

The files we created and edited can be found in the “Code” folder.

The entire code plus framework can be downloaded here:

https://fhooe-my.sharepoint.com/:u:/g/personal/s2010238017_fhooe_at/ES0QqkgmlHJBuv-Dl6GmNSgBjvHqqT9Nt9XyX-p9-blwqA


To run the build, download the files “data”, “SDL2.dll”, and “Final_Build.exe”. Then run the exe.

! If the Volume is not visible try adjusting the “clipping plane z” slider in the UI window in “Entities/Visible node/Material” !

Added files:
ray_marching.fs (Fragment shader that contains the ray_marching algorithm and the jittering, clipping, and transfer functions)


We edited the following files:
application.cpp (created VolumeNode)
material.cpp / material.h (VolumeMaterial logic)
scenenode.cpp / scenenode.cpp (VolumeNode logic)


## Functionality
Model: Change the position, rotation, and scale of the volume

Material:
Step Length: Adjust the length of the ray steps. Smaller steps lead to a higher resolution
Brightness: Adjust the brightness of the volume
Jittering Noise: Displace the rays using a noise texture. This avoids artifacts caused by a very small step length
Jittering Function: Displace the rays using a function. This avoids artifacts caused by a very small step length
Clipping Plane z: Adjust the clipping plane along the z axis
Transfer Function: Apply a transfer function to the volume. This helps to visualize certain parts of the volume better. “Bone” and “Organs” work with the “Abdomen” Volume. “Custom” allows the user to configure their own transfer function using sliders.

Volume: Select a volume to display it

![abdomen](https://user-images.githubusercontent.com/50264516/212962097-9c5e9b2b-1b9a-4a9b-a661-2ed6510c7146.png)
![foot](https://user-images.githubusercontent.com/50264516/212962107-4691080f-c4b0-4ef1-ba90-f3e275698446.png)
![teapot](https://user-images.githubusercontent.com/50264516/212962113-f45c41f9-7e9f-4280-bcfe-bb8b78e405d8.png)
