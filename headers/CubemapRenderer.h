#ifndef H_CUBEMAP_RENDERER
#define H_CUBEMAP_RENDERER

#include <functional>
#include <glew.h>

// Ingemar headers
#include "GL_utilities.h"
#include "LittleOBJLoader.h" 
#include "LoadTGA.h"
#include "VectorUtils4.h" 

class CubemapRenderer {
    public:
        void update(vec3 position, std::function<void(mat4, mat4)> render);
        GLuint getCubemap();
    private:
        static mat4 fov90;
        FBOstruct *cubemap = nullptr;
};

#endif // H_CUBEMAP_RENDERER