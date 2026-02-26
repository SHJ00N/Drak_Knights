#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include "object/game_object.h"
#include "camera.h"
#include "object/interface/renderable.h"
#include "shader.h"
#include "render/renderer/skeletal_mesh_renderer.h"
#include "object/interface/animatable.h"
#include "animation/animator.h"
#include "object/collider/aabb.h"

class World;
class Model;
class Weapon;

enum class ActionState
{
    None,
    Attack1,
    Attack2,
    Attack3,
    Roll
};
enum class MotionState
{
    Idle,
    Walk,
    Run
};

const float PLAYER_SPEED = 4.0f;
const float ACTION_INPUT_BUFFER_TIME = 0.25f; // time window to accept buffered action input

class Player : public GameObject, public Renderable, public Animatable
{
public:
    float Speed;
    bool IsAttacking = false;
    bool IsRunning = false;
    bool IsRolling = false;

    // constructor(s)
    Player(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation = glm::vec3(0.0f), glm::vec2 velocity = glm::vec2(0.0f));

    // override functions
    void Update(const ObjectUpdateContext &context) override;
    // renderable override
    void Render(const struct Frustum &frustum) override;
    void RenderShadow(const struct Frustum& frustum) override;
    // animatable override
    void UpdateAnimation(float dt) override;
    void SetAnimation(Animation* animation, bool force = false) override;
    void AddAnimation(const std::string &name, Animation *animation) override;

    // weapon control
    void AttachWeapon(Weapon *weapon);
    
    // player controls
    void RequestMove(const glm::vec3 direction, bool isRunning);
    void RequestAttack();
    void RequestRoll();

    glm::vec3 GetSoketLocalPosition(const std::string &name);
    glm::vec3 GetSoketGlobalPosition(const std::string &name);
    MotionState GetMotionState() const;
    ActionState GetActionState() const;

private:
    // render member
    SkeletalMeshRenderer m_renderer;
    Animator m_animator3D;
    Shader &m_shader;
    Model &m_model;
    // attachment
    std::unordered_map<std::string, int> m_sokets;
    Weapon *m_weapon = nullptr;
    // aabb collider
    std::unique_ptr<AABB> m_collider;
    // player control related members
    ActionState m_actionState = ActionState::None;
    MotionState m_motionState = MotionState::Idle;
    glm::vec3 m_inputMoveDirection = glm::vec3(0.0f);
    bool m_inputIsRunning = false;
    glm::vec3 m_moveDirection = glm::vec3(0.0f);
    bool m_isRunning = false;
    float m_actionTimer = 0.0f; // timer for actions
    ActionState m_inputActionState = ActionState::None; // to store input action state during performing actions 
    float m_actionInputBufferTime = 0.0f; // time window to accept buffered action input
    // to store action cancel time for each action state
    std::unordered_map<ActionState, float> m_actionCancelTable = 
    {
        {ActionState::Attack1, 1.20f},
        {ActionState::Attack2, 2.0f},
        {ActionState::Attack3, 2.75f},
        {ActionState::Roll, 1.85f}
    };    
    
    // private update functions
    void updateWeaponTransform();
    void updateWorldHeight(const World &world);

    // configure sokets
    const glm::mat4& getSoketMat(const std::string& name);
    void soketConfig();

    // control functions
    void startAction(ActionState action);
    void updateAction(float dt);
    void transitionFromAction();
    void transformFromActionAnimation(float dt);
    void updateMotion();
    void move(Camera &camera, float dt);
    void attack(float dt);
    void roll(float dt);
    void updateAnimation();
    bool canCancelAction() const;
};