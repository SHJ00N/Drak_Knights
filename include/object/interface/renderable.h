#pragma once

class Renderable
{
public:
    bool RenderActive = true;
    bool RenderableDestroyed = false;
    virtual ~Renderable() = default;
    virtual void Render(const struct Frustum& frustum) = 0;
    virtual void RenderShadow(const struct Frustum& frustum) = 0;
};