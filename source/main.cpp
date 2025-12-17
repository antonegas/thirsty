#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#define MAIN
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include <string>
#include <memory>
#include <array>

// Gamepad Motion: https://github.com/JibbSmart/GamepadMotionHelpers
#include "GamepadMotion.h"

// Ingemar headers
#include "GL_utilities.h"
#include "LittleOBJLoader.h" 
#include "LoadTGA.h" 
#include "VectorUtils4.h" 

// Own headers
#include "Bottle.h"
#include "Television.h"
#include "CubemapRenderer.h"

// NES emulator
#include "GLScreen.h"
#include "RomFile.h"
#include "Mapper.h"
#include "Bus.h"
#include "mappers/NROM.h"
#include "Palette.h"

using std::uint64_t;

SDL_Window *window;
SDL_GLContext glContext;

uint64_t const WINDOW_CONFIG = (
    SDL_WINDOW_RESIZABLE |
    SDL_WINDOW_OPENGL |
    SDL_WINDOW_MOUSE_CAPTURE
);

/* Window state */
bool fullscreen = false;
constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 800;

/* Objects */
Bottle bottle;
Television tv;

/* Emulator */
Bus bus;
std::shared_ptr<GLScreen<256, 240>> screen = std::make_shared<GLScreen<256, 240>>(GLScreen<256, 240>());
RomFile rom;
Palette palette;
GLuint nesTexture = 0;
bool emulatorRunning = false;
SDL_DialogFileFilter const romFilter {
    "NES ROM file",
    "nes"
};

/* Gamepads */
typedef struct {
    SDL_JoystickID id = 0;
    vec3 accelerometer{0.0, 0.0, 0.0};
    vec3 gyroscope{0.0, 0.0, 0.0};
} SensorReading;

SensorReading sensorReading;
GamepadMotion gamepadMotion;

/* Other state */
float previousTime = 0.0;
int previousX = 0.0;
float cameraRotationDirection = 0.0;
float cameraRotation = 0.0;

/* Cubemaps */
CubemapRenderer cubemap;

/* Init functions */
bool initGL();
bool initNES();

/* Event handlers */
void handleWindowResize(SDL_WindowEvent *event);
void handleWindowMove(SDL_WindowEvent *event);
void handleMouseMotion(SDL_MouseMotionEvent *event);
void handleGamepadDevice(SDL_GamepadDeviceEvent *event);
void handleGamepadSensor(SDL_GamepadSensorEvent *event);

/* Update functions */
void resize(int width, int height);
void updateSize(int width, int height);
void updateOffset();
void updateBottleRotation(vec3 axis, float angle);
void updateCameraRotation(float delta);
void updateGamepadMotion(float delta);
void update();

/* Rom loading */
void fileDialogCallback(void *userdata, const char *const *filelist, int filter);
void loadRom(std::string path);

/* Render functions */
void render();

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "120");

    if (!SDL_Init(SDL_INIT_GAMEPAD)) {
        SDL_Log("Couldn't initialize gamepad API:, %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_EVENTS)) {
        SDL_Log("Couldn't initialize events: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_SENSOR)) {
        SDL_Log("Couldn't initialize sensors: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("THIRSTY", DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_CONFIG);
    
    if (!window) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!initGL()) {
        return SDL_APP_FAILURE;
    }

    if (!initNES()) {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

bool space_down = false;

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        handleWindowResize((SDL_WindowEvent*) event);
    }

    if (event->type == SDL_EVENT_GAMEPAD_ADDED) {
        handleGamepadDevice((SDL_GamepadDeviceEvent*)event);
    }

    if (event->type == SDL_EVENT_GAMEPAD_SENSOR_UPDATE) {
        handleGamepadSensor((SDL_GamepadSensorEvent*)event);
    }

    if (event->type == SDL_EVENT_WINDOW_MINIMIZED) {
        bus.pause();
    }
    if (event->type == SDL_EVENT_WINDOW_EXPOSED) {
        bus.unpause();
    }

    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
        SDL_KeyboardEvent *keyboardEvent = (SDL_KeyboardEvent*)event;
        if (keyboardEvent->mod & SDL_KMOD_CTRL) {
            // CTRL + KEY
            switch (((SDL_KeyboardEvent*)event)->key) {
                case SDLK_O:
                    if (keyboardEvent->down) SDL_ShowOpenFileDialog(&fileDialogCallback, nullptr, window, &romFilter, 1, 0, false);
                default:
                    break;
            }
        } else {
            switch (((SDL_KeyboardEvent*)event)->key) {
                case SDLK_F10:
                    if (((SDL_KeyboardEvent*)event)->down) {
                        fullscreen = !fullscreen;
                        SDL_SetWindowFullscreen(window, fullscreen);
                    }
                    break;
                case SDLK_UP:
                    bottle.setLevel(bottle.getLevel() + 0.01 * static_cast<float>(keyboardEvent->down));
                    break;
                case SDLK_DOWN:
                    bottle.setLevel(bottle.getLevel() - 0.01 * static_cast<float>(keyboardEvent->down));
                    break;
                case SDLK_LEFT:
                    cameraRotationDirection = -M_PI * static_cast<float>(keyboardEvent->down);
                    break;
                case SDLK_RIGHT:
                    cameraRotationDirection = M_PI * static_cast<float>(keyboardEvent->down);
                    break;
                default:
                    break;
            }
        }
    }

    if (event->type == SDL_EVENT_WINDOW_MOVED) {
        handleWindowMove((SDL_WindowEvent*)event);
    }

    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        handleMouseMotion((SDL_MouseMotionEvent*)event);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    update();
    render();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}

mat4 rotation = IdentityMatrix();
mat4 projection = IdentityMatrix();
mat4 view = lookAtv(
        vec3(0.0, 0.0, 0.75),
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0)
    );
mat4 offset = IdentityMatrix();

bool initGL() {
    glContext = SDL_GL_CreateContext(window);
    
    if (!glContext) {
        SDL_Log("Couldn't create OpenGL context", SDL_GetError());
        return false;
    }
    
    if (!SDL_GL_MakeCurrent(window, glContext)) {
        SDL_Log("Couldn't make context current: %s", SDL_GetError());
        return false;
    }

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        /* Problem: glewInit failed, something is seriously wrong. */
        SDL_Log("Failed to init glew: %s", glewGetErrorString(err));
        return false;
    }

    int width = 0;
    int height = 0;
    
    if (!SDL_GetWindowSize(window, &width, &height)) {
        SDL_Log("Failed to get window size: %s", SDL_GetError());
        return false;
    }

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Bottle::init();
    Television::init();

    resize(width, height);
    updateSize(width, height);
    updateOffset();
    updateBottleRotation({1.0, 0.0, 0.0}, 0.0);
    
    return true;
}

bool initNES() {
    // Load texture
    LoadTGATextureSimple("models/tv/testimage.tga", &nesTexture);
    tv.setScreen(nesTexture);

    // Load palette data
    std::vector<uint8_t> data;
    std::size_t bytesCount;
    uint8_t *bytes = static_cast<uint8_t*>(SDL_LoadFile("models/tv/2C02G_wiki.pal", &bytesCount));

    if (!bytes) {
        SDL_Log("Failed to load Palette file: %s", SDL_GetError());
        return false;
    }

    screen->swap();

    data.resize(bytesCount);
    std::copy(&bytes[0], &bytes[bytesCount], std::begin(data));

    // Connect palette and screen to bus
    // TODO: connect controllers
    palette = Palette(data);
    bus.connectScreen(screen);
    bus.setPalette(palette);

    return true;
}

void handleWindowResize(SDL_WindowEvent *event) {
    resize(event->data1, event->data2);
    updateSize(event->data1, event->data2);
}

void handleWindowMove(SDL_WindowEvent *event) {
    updateOffset();

    bottle.setVelocity({static_cast<float>(event->data1 - previousX) / 10.0f, 0.0, 0.0});
    previousX = event->data1;
}

void handleMouseMotion(SDL_MouseMotionEvent *event) {
    if (!(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LEFT)) return;

    vec3 u{-event->yrel, -event->xrel, 0.0};

    updateBottleRotation(u, Norm(u) * 0.008);
}

void handleGamepadDevice(SDL_GamepadDeviceEvent *event) {
    sensorReading.id = event->which;
    SDL_Log("CONNECTED");

    SDL_Gamepad *gamepad = SDL_OpenGamepad(sensorReading.id);

    SDL_Log("GAMEPAD %s", SDL_GetGamepadName(gamepad));

    if (SDL_GamepadHasSensor(gamepad, SDL_SENSOR_ACCEL)) {
        SDL_Log("HAS ACC");
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_ACCEL, true);
    }

    if (SDL_GamepadHasSensor(gamepad, SDL_SENSOR_GYRO)) {
        SDL_Log("HAS GYRO");
        SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_GYRO, true);
    }
}

void handleGamepadSensor(SDL_GamepadSensorEvent *event) {
    vec3 data{
        event->data[0],
        event->data[1],
        event->data[2]
    };

    if (event->sensor == SDL_SENSOR_ACCEL) {
        sensorReading.accelerometer = data;
    }
    if (event->sensor == SDL_SENSOR_GYRO) {
        sensorReading.gyroscope = data;
    }
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
}

void updateSize(int width, int height) {
    float smallest = std::min(width, height);
    projection.m[0] = smallest / width;
    projection.m[5] = smallest / height;
    float ratio = static_cast<float>(width) / static_cast<float>(height);

    projection = perspective(70, ratio, 0.2, 10.0);
}

void updateOffset() {
    int width;
    int height;
    int x;
    int y;

    SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
    
    if (!displayID) {
        SDL_Log("Failed to get display: %s", SDL_GetError());
        return;
    }
    
    const SDL_DisplayMode *displayMode = SDL_GetDesktopDisplayMode(displayID);

    if (!displayMode) {
        SDL_Log("Failed to get display mode: %s", SDL_GetError());
        return;
    }
    
    if (!SDL_GetWindowSize(window, &width, &height)) {
        SDL_Log("Failed to get window size: %s", SDL_GetError());
        return;
    }

    if (!SDL_GetWindowPosition(window, &x, &y)) {
        SDL_Log("Failed to get window position: %s", SDL_GetError());
        return;
    }

    float wOffset = ((x - displayMode->w / 2) / (float)width + 0.5) * 0.5;
    float hOffset = ((y - displayMode->h / 2) / (float)height + 0.5) * 0.5;

    // NOTE: offset is unused at the moment
    offset = T(-wOffset, hOffset, 0.0);
}

void updateBottleRotation(vec3 axis, float angle) {
    rotation = ArbRotate(Ry(-cameraRotation) * axis, angle) * rotation;

    bottle.setRotation(rotation);

    bottle.setVelocity({angle * 3.0f, 0.0, 0.0});
}

void updateCameraRotation(float delta) {
    cameraRotation += cameraRotationDirection * delta;
    vec4 cpos = Ry(cameraRotation) * vec4(0.0, 0.0, 0.75, 1.0);

    view = view = lookAtv(
        vec3(cpos.x, cpos.y, cpos.z),
        vec3(0.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0)
    );
}

void render() {
    GLfloat time = SDL_GetTicksNS() / 10e9;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw to nes texture
    if (emulatorRunning) {
        screen->draw(nesTexture);
        glGenerateTextureMipmap(nesTexture);
    } 

    // Update cubemap
    cubemap.update(vec3{0.0, 0.0, 0.0}, [&time](mat4 view, mat4 projection) {
        tv.render(time, view, projection);
    });
    bottle.setCubemap(cubemap.getCubemap());

    // NOTE: Since the bottle needs refraction it should be drawn last.
    tv.render(time, view, projection);
    bottle.render(time, view, projection);

    // Output to screen.
    SDL_GL_SwapWindow(window);
}

void update() {
    uint64_t ticks = SDL_GetTicksNS();
    float time = ticks / 10e9;

    if (previousTime == 0.0) {
        previousTime = time;
        return;
    }

    float delta = time - previousTime;
    previousTime = time;

    // Update objects
    // updateGamepadMotion(delta);
    bottle.update(delta);
    bus.update(ticks);

    updateCameraRotation(delta);
}

void updateGamepadMotion(float delta) {
    float gyroX = 180.0 * sensorReading.gyroscope.x / M_PI;
    float gyroY = 180.0 * sensorReading.gyroscope.y / M_PI;
    float gyroZ = 180.0 * sensorReading.gyroscope.z / M_PI;
    float accelerometerX = sensorReading.accelerometer.x / SDL_STANDARD_GRAVITY;
    float accelerometerY = sensorReading.accelerometer.y / SDL_STANDARD_GRAVITY;
    float accelerometerZ = sensorReading.accelerometer.z / SDL_STANDARD_GRAVITY;

    gamepadMotion.ProcessMotion(gyroX, gyroY, gyroZ, accelerometerX, accelerometerY, accelerometerZ, delta);

    GLfloat x = 0.0f;
    GLfloat y = 0.0f;
    GLfloat z = 0.0f;
    GLfloat w = 0.0f;

    gamepadMotion.GetOrientation(w, x, y, z);

    // Quaternion to rotation matrix.
    mat4 rotation = {
        1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y - w * z), 2.0f * (x * z + w * y), 0.0f,
        2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z - w * x), 0.0f,
        2.0f * (x * z - w * y), 2.0f * (y * z + w * x), 1.0f - 2.0f * (x * x + y * y), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    bottle.setRotation(4.0 * rotation);
}

void fileDialogCallback(void *userdata, const char *const *filelist, int filter) {
    if (!filelist) {
        SDL_Log("File could not be opened: %s", SDL_GetError());
        return;
    }
    if (!(*filelist)) {
        SDL_Log("No file selected");
        return;
    }
    loadRom(filelist[0]);
}

void loadRom(std::string path) {
    SDL_Log("File selected: %s", path.c_str());
    
    std::vector<uint8_t> data;
    std::size_t bytesCount;
    uint8_t *bytes = static_cast<uint8_t*>(SDL_LoadFile(path.c_str(), &bytesCount));

    if (!bytes) {
        SDL_Log("Failed to load ROM file: %s", SDL_GetError());
    }

    data.resize(bytesCount);
    std::copy(&bytes[0], &bytes[bytesCount], std::begin(data));

    rom = RomFile(data);

    SDL_Log("ROM SIZE: %u", bytesCount);
    SDL_Log("ROM PRGROM SIZE: %u", rom.getPrgromSize());
    SDL_Log("ROM CHRROM SIZE: %u", rom.getChrromSize());

    bus.insertCart(rom.getMapper());

    // Signal that emulator is running.
    emulatorRunning = true;
}