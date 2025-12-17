#include <SDL3/SDL.h>
#include <glew.h>

#include "CubemapRenderer.h"

#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "VectorUtils4.h"
#include "LoadTGA.h"

mat4 CubemapRenderer::fov90 = mat4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, -(10.0f + 0.2f) / (10.0f - 0.2f), -2.0f * 10.0f * 0.2f / (10.0f - 0.2f),
    0.0f, 0.0f, -1.0f, 0.0f
);

void CubemapRenderer::update(vec3 position, std::function<void(mat4, mat4)> render) {
    // Init FBO if not already
    if (!cubemap) {
        cubemap = initCubemapFBO(1000, 1000, 0);
    }

    // Set active FBO to cubemap.
    useFBO(cubemap, 0, 0);
    constexpr size_t SIDES = 6;

    GLenum cubeSides[SIDES] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	};

	vec3 upVectors[SIDES] = {
		{0, 1, 0},
		{0, 1, 0},
		{0, 1, 0},
		{0, 1, 0},
		{0, 0, 1},
		{0, 0, 1},
	};

	vec3 directions[SIDES] = {
		{1, 0, 0},
		{-1, 0, 0},
		{0, 0, -1},
		{0, 0, 1},
		{0, 1, 0},
		{0, -1, 0},
	};
    
    for (size_t i = 0; i < SIDES; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cubeSides[i], cubemap->texid, 0);
        CHECK_FRAMEBUFFER_STATUS();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 view = lookAt(
            position,
            position + directions[i],
            upVectors[i]
        );

        render(view, fov90);
    }
    
    // Reset FBO to be viewport.
    useFBO(0, 0, 0);
}

GLuint CubemapRenderer::getCubemap() {
    return cubemap->texid;
}