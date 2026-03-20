#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "object/game_object.h"
#include "object/interface/animatable.h"
#include "object/interface/collidable.h"
#include "object/interface/renderable.h"
#include "object/interface/renderable.h"
#include "object/interface/damageable.h"
#include "object/interface/socket.h"
#include "object/collider/box_collider.h"
#include "render/renderer/skeletal_mesh_renderer.h"
#include "object/enemy/enemy_BT.h"

class Shader;
class Model;
class World;
class Weapon;

class Enemy : public GameObject, public Renderable, public Animatable, public Collidable, public Damageable, public ISocket
{
public:
    int Health;
    bool IsHit = false;
    bool IsDeath = false;
    bool IsAttacking = false;
    // constuctor
    Enemy(Model &model, Shader &shader, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation = glm::vec3(0.0f), glm::vec2 velocity = glm::vec2(0.0f), Layer layout = Layer::Enemy);

    // override functions
    // game object
    void Init() override;
    void Update(const ObjectUpdateContext &context) override;
    // renderable
    void Render(const struct Frustum &frustum) override;
    void RenderShadow(const struct Frustum &frustum) override;
    // damageable
    void TakeDamage(int damage) override;
    // socket override
    void SocketConfig() override;

    void EquipWeapon(Model &model, Shader &shader, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
    void SetupBehaviorTree(std::vector<glm::vec3> &wayPoints, GameObject &target);

    Weapon* GetWeapon() const;

private:
    // render member
    SkeletalMeshRenderer m_renderer;
    Shader &m_shader;
    Model &m_model;

    // aabb and aixs correction
    std::unique_ptr<AABB> m_aabb;
    glm::vec3 axisFix = glm::vec3(-90.0f, 0.0f, 0.0f);

    // weapon
    Weapon *m_weapon;

    // behavior tree
    std::unique_ptr<EnemyBT> m_BT;

    float m_deathTimer = 0.0f;

    // set height from world
    void updateWorldHeight(const World &world);
};