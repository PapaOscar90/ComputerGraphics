# README
## Description
Following the instructions, we began by implementing the required sections to successfully trace the spheres on the screen. The bulk of the time was taken up by getting the point P to correctly compute. Once this point is known, the vectors were computed from the location and relative to the light source and surface normal (that we compute).
It took some tweaking to get the images to correctly match, but we found out that the ambiant light is not a white addition, but just an increase in the object's own surface matierials directly. So a red sphere gets brighter by adding more of itself instead of adding "white light" which white-washes the image. Which in itself is also a good discovery because we can adjust the white-balance of our images later.
Each shape is drawn by calculating the implicit equation where the ray either intersects once, twice, or not at all. In the case that they intersect twice, the minimum is taken to be the correct point for rending as it is in front of the other. The difficulty in adding the shapes lays in interpreting each mathematical equation in code. Usually there is an obvious way to represent each object.
We also parallelized the rendering of the image so that multi-thread computers can drastically speed up the render process.

##Screenshots:
### scene01_no_shading
This is a purely intersect only render. If the ray collides with a sphere, draw the color that matches at the point P.

### scene01_diffuse
This image is the result of our previous intersection with only the diffuse light added.

### scene01_full_phong
This image is the same as the previous, but with the specular and ambiant light added. Note that the position of the light is clearly identified by the position of the highlights, and the different materiels of each sphere causing the radius of the highlights to be large or small.

### scene02_no_shading
This is the same as before, but a new scene. Note how the "black" sphere is orange when it's diffuse doesn't effect the light hitting it.

### scene02_diffuse
Also the same as the other scene. Nothing new to highlight besides the "black" sphere.

### scene02_full_phong
The spheres are now shiny.

### scene03_triangles
Here we demonstrate the rendering of triangles. Note that there is one triangle in the center of the screen that is parallel to the Eye's viewpoint. As a result the triangle is invisible.

### scene04_plane
With the addition of planes, we decided to make a "sunset" style render. 

### scene05_cylinder
We demonstrate the rendering of a cylinder with a greek style columned building.

### scene06_obj_grouped_spheres
We render 5 spheres using imported meshes.

### scene07_cubes
We render 2 cubes imported from obj files. Nothing spectacular to note. They are cubes. However, we discovered that we had to export obj files from Blender with triangulate faces toggled on or else we couldn't render the mesh. As this is not the default export option from Blender 2.8 this caused some problems.

## Feedback
We enjoyed this lab, though it took a lot of time to look up and derive each equation for the shapes.

# Raytracer C++ framework for Introduction to Computer Graphics

This is a C++ framework for a raytracer. It is created for the Computer
Science course "Introduction to Computer Graphics" taught at the
University of Groningen.

This framework is inspired by and uses (some) code of the raytracer framework of
Bert Freudenberg which unfortunately is no longer available.

## Compiling the code

We provided a [CMakeLists.txt](CMakeLists.txt) file to be used with `cmake`.
This can be used in the following way (on the command line/shell):
```
# cd into the main directory of the framework
mkdir build   # create a new build directory
cd build      # cd into the directory
cmake ..      # creates a Makefile
# Comipiling the code can then be done with:
make
# or
make -j4      # replacing 4 with the number of cores of your pc
```
**Note!** After adding new `.cpp` files (when adding new shapes)
`cmake ..` needs to be called again or you might get linker errors.

## Running the Raytracer
After compilation you should have the `ray` executable.
This can be used like this:
```
./ray <path to .json file> [output .png file]
# when in the build directory:
./ray ../Scenes/scene01.json
```
Specifying an output is optional and by default an image will be created in
the same directory as the source scene file with the `.json` extension replaced
by `.png`.

## Description of the included files

### Scene files
* `Scenes/*.json`: Scene files are structured in JSON. If you have never
    worked with json, please see [here](https://en.wikipedia.org/wiki/JSON#Data_types,_syntax_and_example)
    or [here](https://www.json.org/).

    Take a look at the provided example scenes for the general structure.
    You are free (and encouraged) to define your own scene files later on.

### The raytracer source files (Code directory)

* `main.cpp`: Contains main(), starting point. Responsible for parsing
    command-line arguments.

* `raytracer.cpp/.h`: Raytracer class. Responsible for reading the scene
    description, starting the raytracer and writing the result to an image file.

* `scene.cpp/.h`: Scene class. Contains code for the actual raytracing.

* `image.cpp/.h`: Image class, includes code for reading from and writing to PNG
    files.

* `light.h`: Light class. Plain Old Data (POD) class. Colored light at a
    position in the scene.

* `ray.h`: Ray class. POD class. Ray from an origin point in a direction.

* `hit.h`: Hit class. POD class. Intersection between an `Ray` and an `Object`.

* `object.h`: virtual `Object` class. Represents an object in the scene.
    All your shapes should derive from this class. See

* `shapes (directory/folder)`: Folder containing all your shapes.

* `sphere.cpp/.h (inside shapes)`: Sphere class, which is a subclass of the
    `Object` class. Represents a sphere in the scene.

* `example.cpp/.h (inside shapes)`: Example shape class. Copy these two files
    and replace/rename **every** instance of `Example` `example.h` or `EXAMPLE`
    with your new shape name.

* `triple.cpp/.h`: Triple class. Represents a 3-dimensional vector which is
    used for colors, points and vectors.
    Includes a number of useful functions and operators, see the comments in
    `triple.h`.
    Classes of `Color`, `Vector`, `Point` are all aliases of `Triple`.

* `objloader.cpp/.h`: Is a similar class to Model used in the OpenGL
    exercises to load .obj model files. It produces a std::vector
    of Vertex structs. See `vertex.h` on how you can retrieve the
    coordinates and other data defined at vertices.

### Supporting source files (Code directory)

* `lode/*`: Code for reading from and writing to PNG files,
    used by the `Image` class.
    lodepng is created by Lode Vandevenne and can be found on
    [github](https://github.com/lvandeve/lodepng).
* `json/*`: Code for parsing JSON documents.
    Created by Niels Lohmann and available under the MIT license on
    [github](https://github.com/nlohmann/json).
    **Recommended:** Especially take a look at their README for more
    info on how to work with json files.
