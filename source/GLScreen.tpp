#ifndef T_GLSCREEN
#define T_GLSCREEN

#ifndef H_GLSCREEN
#error __FILE__ should only be included from SDLScreen.h.
#endif // H_SDLSCREEN

#include <glew.h>

#include "Screen.h"
#include "GLScreen.h"

template <std::size_t W, std::size_t H>
void GLScreen<W, H>::draw(GLuint texture) {
    typename Screen<W, H>::Buffer buffer = GLScreen::buffers[0];

    // TODO: something is not working here.
    glTextureSubImage2D(
        texture,
        0,
        0,
        0,
        W,
        H,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        buffer.data()
    );
}

#endif // T_GLSCREEN