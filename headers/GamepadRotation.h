#include <VectorUtils4.h>
#include <SDL3/SDL_gamepad.h>
#include <cstdint>

using std::uint64_t;

typedef struct {
    SDL_JoystickID id{0};
    vec3 rotation{0.0, 0.0, 0.0};
    vec3 gyroscope{0.0, 0.0, 0.0};
    vec3 accelerometer{0.0, 0.0, 0.0};

    void update(float delta);
    mat4 getMatrix();
} GamepadRotation;