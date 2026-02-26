#pragma once

class SkyBoxPass
{
public:
    SkyBoxPass();
    ~SkyBoxPass();
    void Render(unsigned int envCubeMap);

private:
    unsigned int m_cubeVAO = 0;
    unsigned int m_cubeVBO = 0;

    void renderCube();
};