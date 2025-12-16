#ifndef H_TV
#define H_TV

#include <glew.h>

// Ingemar headers
#include "GL_utilities.h"
#include "LittleOBJLoader.h" 
#include "LoadTGA.h"
#include "VectorUtils4.h" 

class Television {
    public:
        static void init();

        void setScreen(GLuint texture);
        void update(float delta);
        void render(float time, mat4 projection);
    private:
        // Shaders
        static GLuint frameShader;
        static GLuint screenShader;

        // Textures
        static GLuint screenTexture;

        // Models
        static Model *frame;
        static Model *screen;

        // Other
        static bool initialized;

        // Colors
        vec4 frameColor{0.0, 0.0, 0.0, 1.0};
};

#endif // H_TV