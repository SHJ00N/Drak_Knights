#include "render/pass/ui_pass.h"
#include "scene/scene.h"

UIPass::UIPass(unsigned int width, unsigned int height)
{
    m_textRenderer = std::make_unique<TextRenderer>(width, height);

    m_textRenderer->Load("resources/font/Poly-Regular.otf", 24);
}

void UIPass::Render(Scene *scene)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(const auto& text : scene->GetUITexts())
    {
        m_textRenderer->RenderText(text.text, text.x, text.y, text.scale, text.color);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}