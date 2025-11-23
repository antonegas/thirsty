#ifndef H_BOTTLE
#define H_BOTTLE

#include <array>
#include <glad/glad.h>

#include "VectorUtils4.h"

class Bottle {
    public:
        static void init();

        void setPosition(vec3 position);
        void setRotation(vec3 rotation);
        void setVelocity(vec3 velocity);
        void setAngular(vec3 angular);
        void setLevel(float level);
        void update(float delta);
        void render(mat4 projection);
    private:
        static GLuint glassProgram;
        static GLuint liquidProgram;
        static Model *outside;
        static Model *inside;

        vec3 normal{0.0, 1.0, 0.0};
        vec3 position{0.0, 0.0, 0.0};
        vec3 rotation{0.0, 0.0, 0.0};
        vec3 velocity{0.0, 0.0, 0.0};
        vec3 angular{0.0, 0.0, 0.0};
        float level{0.0};

        // Hard coded, better not change bottle model
        const std::array<vec3, 8> flat = {{
            {-0.013021, 0.173285, 0.0},
            {-0.013135, 0.10971, 0.0},
            {-0.030462, 0.074736, 0.0},
            {-0.030354, -0.112885, 0.0},
            {0.030354, -0.112885, 0.0},
            {0.030462, 0.074736, 0.0},
            {0.013135, 0.10971, 0.0},
            {0.013021, 0.173285, 0.0}
        }};

        float calculatePlanePoint();
};

#endif // H_BOTTLE