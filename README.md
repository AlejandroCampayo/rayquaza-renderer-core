# Rayquaza Renderer Core

**Rayquaza** is a physically-based renderer built on the **Lightwave** framework.  
It was developed as part of the [Computer Graphics course](https://graphics.cg.uni-saarland.de) at **Saarland University**.

---

## Features

- Support for triangle meshes (`.ply`), rectangles, sphere and mandelbulb primitives
- BVH acceleration with binning
- Multithreaded rendering
- Physically-based materials: Diffuse, Conductor, Dielectric, Principled BSDFs
- Texture support: Image and checkerboard
- Light types: Point, Directional, Area, and Image-based lighting
- Next Event Estimation and full Path Tracing modes
- Halton sampling
- Image denoising (via Intel OpenImageDenoise)
- Bloom and glare postprocessing effects.
- Alpha masking and shading normals

🔗 See [this demo](https://alejandrocampayo.github.io/rayquaza-renderer-website/) for an in-depth example of a complex scene rendered with Rayquaza.

---

## Getting Started

### Prerequisites

Most dependencies are bundled with this repository.  
However, **Intel OpenImageDenoise (OIDN)** may behave differently on non-MacBook environments. Rayquaza works without OIDN, but output images will be noisier.

### Build Instructions

```bash
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

You can also build using standard Makefiles:

```bash
cmake -S . -B build -G "Unix Makefiles"
cmake --build build --parallel
```

---

## Rendering Scenes

Scenes are encoded as `.xml` files.  
To render the provided sample scenes:

```bash
./build/Rayquazing tests/practical_1/mesh_bunny_flat.xml
```

To render your own Blender scenes, install the **Lightwave Blender Exporter** from:  
`blender_exporter/lightwave_blender.zip`

The output image will be saved in the same directory as the `.xml` scene file.

---

## Real-Time Preview

For live image updates during rendering, you can use [tev](https://github.com/Tom94/tev/releases):

1. Install and launch tev
2. Run your scene in Rayquaza
3. The rendered image will update in tev in real-time

---

## Acknowledgements

Rayquaza is built upon the **Lightwave** renderer, originally developed by  
[Alexander Rath](https://graphics.cg.uni-saarland.de/people/rath.html), with contributions from  
[Ömercan Yazici](https://graphics.cg.uni-saarland.de/people/yazici.html) and  
[Philippe Weier](https://graphics.cg.uni-saarland.de/people/weier.html).

We were also inspired by [Nori](https://wjakob.github.io/nori/), an educational renderer by Wenzel Jakob.

Special thanks to the developers of our dependencies:  
[ctpl](https://github.com/vit-vit/CTPL), [miniz](https://github.com/richgel999/miniz),  
[stb](https://github.com/nothings/stb), [tinyexr](https://github.com/syoyo/tinyexr),  
[tinyformat](https://github.com/c42f/tinyformat), [pcg32](https://github.com/wjakob/pcg32),  
and [Catch2](https://github.com/catchorg/Catch2).
