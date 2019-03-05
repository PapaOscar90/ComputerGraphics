This is a README file

## Changlog
Changes were made in the order of, and method specified, in the problem statement. Some are detailed below, but mostly detailed in comments directly above the relevent code. The code extends from our previous knowledge, and we assume we do not need to re-explain every detail, such as VAO/VBO creation process. Only the parts relevent to this assignment will have more detailed comments.

### Normal Shader
  - Change references to color to normal where necessary.
  - Store additional normal transform data.
  - Pass this data to the shaders.
  - Apply normal data in vertex shader.
  - Normalize color modifier in fragment shader.

## Questions
### Normal Shader
  - Without the normal uniform applied in the vertex shader, the cat has vibrant bands of color dominating particular faces. Its right sides are a vibrant blue, its front-facing sides are a vibrant red, its top is a vibrant green, its left sides are black, as are its back and bottom-facing sides. This colors of the sides remain constant irrespective of the orientation of the cat (e.g. in its default orientation or on its side the cats belly is black). When the normal uniform is applied the cat seems to shimmer as it rotates. The colors are now dependent on the orientation of the cat. In its default orientation the cat's face is red and when the cat is reoriented so that it's belly faces the right, it's belly turns red. As the cat rotates, the normal colors stay relative to the global coordinates.

### glTexParameteri()
We played around with the parameters randomly at first, and didn't see any differences. However, after researching them further, we now know why. The texture we are using is mapped directly onto the mesh, and only maps one way. It either works, or it doesn't. From the research we did, we found the following:

* GL_REPEAT 
**This will repeat the textures after the edge of the texture is reached. This is often visible in games for grass textures. There is a large amount of repeating texture from a small input file.
* GL_CLAMP_TO_EDGE
** This will stop at the last pixel when the edge is reached.

Wrap S/T with the previous modes changes the vertical/horizontal parts individually. For example, if you set Wrap S to GL_CLAMP, and Wrap_T to GL_CLAMP to a checkerboard texture that is smaller than the object being textured, will result in the checkboard pattern, and long strips where the last pixels of the edge of the texture are continued. If they mode is set to GL_REPEAT, the checkboard pattern will just continue on and on until the end of the object being textured.

Of course, there is a bit more depth to this than written here, but we feel it isn't needed to lecture you, the reader, about things that are directly in the docs for this function. Creating a texture for an object requires planning and a few choices to best texture your object without needed unneccisary resources.

### Screenshots
#### Cat_Textured_*
These screenshots show the differences between the two texture shader models. You can plainly see the difference where there is more fine details, for example the head.

#### Application_of_Normal
These screen shots display what was described above, with the different results of normal application. Whether it is local to the model, or to the global frame.

#### Wrapped_In_RUG
For fun, we wrapped the cat in the RUG flag. You can really see the differences between the light and dark side on this texture.

## Controls
Controls are the default UI elements given to us from the startpoint.

## Feedback
Possible to give GIF on nestor of expected outputs. Our model looks like the expected, but can change depending on distance. We are pretty sure this is how it was meant to be, since the vertices are becoming tighter, causing the interpolation to be stronger. But we would have been happier if we knew if we had it exactly correct, since we can easily change it to however it is needed to be.

