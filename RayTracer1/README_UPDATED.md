# README UPDATE
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
Here we demonstrate the rendering of triangles.

### scene04_plane
With the addition of planes, we decided to make a "sunset" style render. 

### scene05_cylinder
We demonstrate the rendering of a cylinder with a greek style columned building.

### scene06_obj_grouped_spheres
We render 5 spheres using imported meshes. We found that we must use object files of a certain version of blender or else they do not render correctly.

### scene07_cubes
We render 2 cubes. Nothing spectacular to note. They are cubes.

## Feedback
We enjoyed this lab, though it took a lot of time to look up and derive each equation for the shapes.
