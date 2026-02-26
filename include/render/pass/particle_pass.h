#pragma once

#include <glad/glad.h>

class Scene;

class ParticlePass
{
public:
    void Render(Scene *scene, unsigned int depthTexture, int width, int height);
};