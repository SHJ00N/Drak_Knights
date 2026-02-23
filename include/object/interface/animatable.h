#pragma once

class Animation;

class Animatable
{
public:
    virtual void UpdateAnimation(float dt) = 0;
    virtual void SetAnimation(Animation* animation, bool force = false) = 0;
    virtual void AddAnimation(const std::string &name, Animation *animation) = 0;
    virtual ~Animatable() = default;
};