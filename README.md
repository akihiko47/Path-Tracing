# Simple Path Tracer
![banner](output/banner.png)

Multithreaded path tracer with [YAML](https://yaml.org/) scenes description. 
This format is human-readable, which makes it easy to modify and create new scenes. 

## Features
- Multithreading
- [YAML](https://yaml.org/) scenes description
- Bilateral filtering
- Cross platform
- Lambertian diffuse
- Reflections and refractions
- Stratification and antialiasing

## How to build
*(In root directory after downloading or copying)*
```
mkdir build
cd build/
cmake ..
```
Build files will be in `build/` folder. 
After that you can use `make` or launch solution in `Visual Studio`. 
Alternatively, you can use cmake `cmake --build .` to build this project.

## Optimization thoughts
Right now this path tracer is incredible inefficient. 
So far the available primitives are spheres and quads. 
There is no support of structures like BVH or KD-trees, which makes rendering models almost impossible. 
Also, scene data such as materials and textures is not cache coherent, 
but I tested method with more cache-uniform data and it gave no more than a 10% speed boost. 
This project is rather a proof of concept to later port these methods to GPU.