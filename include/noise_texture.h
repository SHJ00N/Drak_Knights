#pragma once

class NoiseTexture
{
public:
    NoiseTexture();
    ~NoiseTexture();
    void Generate(int width, int height);
    void Bind(unsigned int unit) const;
private:
    unsigned int m_id;
    unsigned int m_width, m_height;
};