# Simple Path Tracer
![banner](output/banner.png)

Multithreaded path tracer with [YAML](https://yaml.org/) scenes description. 
This format is human-readable, which makes it easy to modify and create new scenes. 

## Features
- Multithreading
- Cross platform
- [YAML](https://yaml.org/) scenes description
- Normal mapping
- Bilateral filtering
- Defocus blur
- Reflections and refractions
- Cubemaps and panoramic textures
- Stratification and antialiasing

## How to build
*(In root directory after downloading or copying)*
```bash
mkdir build
cd build/
cmake ..
```
Build files will be in `build/` folder. 
After that you can use `make` or launch solution in `Visual Studio`. 
Alternatively, you can use cmake `cmake --build .` to build this project.

## Usage
This project should be used as command line application. 
After building you will see executable file `PathTracing` that can be launched from terminal. 
If you launch this executable without arguments you will see something like this:
```bash
=== Akihiko Path Tracer ===

Usage:   ./PathTracing <scene-name>
Example: ./PathTracing example

Usage:   ./PathTracing filter <image-name> <kernel-size> <sigma> <b-sigma>
Example: ./PathTracing filter example-scene.png 15 10 0.1

None: default search directories are 'project-root/scenes/' and 'project-root/output/'
(but you can specify absolute path to yaml scene file or image)
Resulting image will be saved in 'project-root/output/'

For more info see README

===========================
```
### Rendering mode
This mode will render [YAML](https://yaml.org/) scene file to png image. 
Default search directory for `.yaml` files is `scenes/` directory inside project root, 
but you can specify absolute path. 
You don't need to specify `.yaml` extension for file. 
Image will be saved in `output/` directory with name specified inside scene file in output section.
```bash
./PathTracing <scene-name>
```

You can try to render example scene using this command: 
```bash
./PathTracing example
```

## Optimization thoughts
Right now this path tracer is incredible inefficient. 
So far the available primitives are spheres and quads. 
There is no support of structures like BVH or KD-trees, which makes rendering models almost impossible. 
Also, scene data such as materials and textures is not cache coherent, 
but I tested method with more cache-uniform data and it gave no more than a 10% speed boost. 
This project is rather a proof of concept to later port these methods to GPU.