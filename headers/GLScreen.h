#ifndef H_GLSCREEN
#define H_GLSCREEN

#include <cstdint>
#include <array>
#include <glew.h>

#include "../Screen.h"

using std::uint8_t;

template <std::size_t W, std::size_t H>
class GLScreen : public Screen<W, H> {
    public:
        void draw(GLuint texture);
};

#include "../source/GLScreen.tpp"

#endif // H_GLSCREEN