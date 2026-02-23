#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "object/entity.h"
#include "object/interface/renderable.h"
#include "render/renderer/static_mesh_renderer.h"
#include "object/collider/aabb.h"

class Model;
class Shader;
class Player;

class Weapon : public Entity, public Renderable
{
public:
    // constructor
    Weapon(Model &model, Shader &shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    // override functions
    void Render(const struct Frustum& frustum) final;
    void RenderShadow(const struct Frustum& frustum) final;

    // setter
    void SetOwner(Player *owner);
private:
    Shader &m_shader;
    Model &m_model;
    StaticMeshRenderer m_renderer;
    Player *m_owner = nullptr; // the player who holds this weapon
    // collider
    std::unique_ptr<AABB> m_collider;
};