#pragma once

#include <glm/glm.hpp>
#include "object/entity.h"

struct ObjectUpdateContext
{
    float deltaTime;
    class World *world;
    class Camera *camera;
};

class GameObject : public Entity
{
public:
    // object state
    glm::vec2 Velocity;
    bool Destroyed;
    
    virtual void Update(const ObjectUpdateContext &context) = 0;
    virtual ~GameObject() = default;
};