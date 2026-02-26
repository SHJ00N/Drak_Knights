#include "noise_texture.h"

#include <glad/glad.h>
#include <random>
#include <vector>

NoiseTexture::NoiseTexture()
{
    glGenTextures(1, &m_id);
}

NoiseTexture::~NoiseTexture()
{
    glDeleteTextures(1, &m_id);
}

void NoiseTexture::Generate(int width, int height)
{
    m_width = width;
    m_height = height;

    // create vector container and configure random floats
    std::default_random_engine generator;
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
    
    std::vector<float> data(width * height);
    for(int i = 0; i < width * height; ++i)
    {
        data[i] = randomFloats(generator);
    }

    // generate texture
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void NoiseTexture::Bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}
