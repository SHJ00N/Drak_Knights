#pragma once

#include "render/renderer/text_renderer.h"

#include <memory>

class UIPass
{
public:
    UIPass(unsigned int width, unsigned int height);
    void Render(class Scene *scene);

private:
    std::unique_ptr<TextRenderer> m_textRenderer;
};