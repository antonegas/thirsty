#ifndef H_BOTTLE
#define H_BOTTLE

#include <glew.h>

// Ingemar headers
#include "GL_utilities.h"
#include "LittleOBJLoader.h" 
#include "LoadTGA.h"
#include "VectorUtils4.h" 

class Bottle {
    public:
        static void init();

        void setVelocity(vec3 velocity);
        void setRotation(mat4 rotation);
        void setTranslation(mat4 translation);
        void setLevel(float level);
        void setCubemap(GLuint cubemap);
        float getLevel();
        void update(float delta);
        void render(float time, mat4 view, mat4 projection);
    private:
        // Shaders
        static GLuint liquidShader;
        static GLuint glassShader;

        // Textures
        static GLuint lut;
        GLuint cubemap = 0;

        // Models
        static Model *outside;
        static Model *inside;

        // Other
        static bool initialized;

        vec3 velocity{0.0, 0.0, 0.0};
        mat4 rotation = IdentityMatrix();
        mat4 translation = IdentityMatrix();
        float level{0.5};
        float foam{0.0};
        float radius{0.1737944187826525}; // Hardcoded

        // Colors
        vec4 glassColor{0.0, 0.3, 0.0, 0.1};
        vec4 liquidFrontColor{0.64313725, 0.49019607, 0.49019607, 1.0};
        vec4 liquidBackColor{0.65098039, 0.61568627, 0.56862745, 1.0};

        // Helper functions
        float calculateAngle();
};

#endif // H_BOTTLE