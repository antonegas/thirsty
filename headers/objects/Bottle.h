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

        // void setPosition(vec3 position);
        // void setRotation(vec3 rotation);
        void setVelocity(vec3 velocity);
        // void setAngular(vec3 angular);
        void setRotation(mat4 rotation);
        void rotate(mat4 rotation);
        void setLevel(float level);
        void update(float delta);
        void render(float time, mat4 projection);

        private:
        static GLuint shaderProgram;
        static Model *outside;
        static Model *inside;
        // static GLuint glassProgram;
        // static GLuint liquidProgram;
        static bool initialized;

        // vec3 normal{0.0, 1.0, 0.0};
        // vec3 position{0.0, 0.0, 0.0};
        // vec3 rotation{0.0, 0.0, 0.0};
        vec3 velocity{0.0, 0.0, 0.0};
        // vec3 angular{0.0, 0.0, 0.0};
        mat4 rotation = IdentityMatrix();
        float level{0.0};

        // Colors
        vec4 glassColor{0.0, 0.3, 0.0, 0.1};
        vec4 liquidBackColor{0.65098039, 0.61568627, 0.56862745, 1.0};
        vec4 liquidFrontColor{0.64313725, 0.49019607, 0.49019607, 1.0};
};

#endif // H_BOTTLE