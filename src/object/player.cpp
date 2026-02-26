#include "object/player.h"
#include "world/world.h"
#include "frustum.h"
#include "object/weapon.h"

#include <iostream>

float NormalizeAngle(float angle);

Player::Player(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec2 velocity) : Speed(PLAYER_SPEED), m_model(model), m_shader(shader)
{
    // set transform info
    transform.SetLocalPosition(position);
    transform.SetLocalScale(size);
    transform.SetLocalRotation(rotation);
    updateSelfAndChild();
    // create collider
    m_collider = std::make_unique<AABB>(m_model);
    // configure soket data
    soketConfig();
}

#pragma region soket
void Player::soketConfig()
{
    m_sokets["RightHand"] = 88;
    m_sokets["Center"] = 27;
}

glm::vec3 Player::GetSoketLocalPosition(const std::string &name)
{
    if(m_sokets.find(name) == m_sokets.end()) return glm::vec3(0.0f);
    return glm::vec3(getSoketMat(name)[3]);
}

glm::vec3 Player::GetSoketGlobalPosition(const std::string &name)
{
    if(m_sokets.find(name) == m_sokets.end()) return glm::vec3(0.0f);
    glm::mat4 boneGlobalModel = transform.GetModelMatrix() * getSoketMat("center");
    return glm::vec3(boneGlobalModel[3]);
}

const glm::mat4& Player::getSoketMat(const std::string &name)
{
    const int soket = m_sokets[name];
    const auto& bones = m_animator3D.GetGlobalBoneMatrices();
    return bones[soket];
}
#pragma endregion

#pragma region override_functions
void Player::Update(const ObjectUpdateContext &context)
{
    float dt = context.deltaTime;
    
    UpdateAnimation(dt);
    
    // update action
    updateAction(dt);
    // update motion and move 
    updateMotion();
    move(*context.camera, dt);
    updateAnimation(); // update animation based on motion and action state
    
    updateWorldHeight(*context.world);  // update height based on world height

    updateSelfAndChild();
    updateWeaponTransform();
}

void Player::UpdateAnimation(float dt)
{
    m_animator3D.UpdateAnimation(dt);
}
void Player::SetAnimation(Animation* animation, bool force)
{
    if(animation == nullptr) return;
    m_animator3D.PlayAnimation(animation, force);
}
void Player::AddAnimation(const std::string &name, Animation *animation)
{
    m_animator3D.registAnimation(name, animation);
}
#pragma endregion

#pragma region render
void Player::Render(const Frustum &frustum)
{
    // inside frsutum
    if (m_collider->isOnFrustum(frustum, transform))
    {
        m_renderer.Draw(m_shader, transform, m_model, m_animator3D);
    }

    // draw children
    for (auto&& child : children)
    {
        if (auto* renderable = dynamic_cast<Renderable*>(child.get()))
            renderable->Render(frustum);
    }
}

void Player::RenderShadow(const Frustum &frustum)
{
    // inside frsutum
    if (m_collider->isOnFrustum(frustum, transform))
    {
        m_renderer.DrawShadow(transform, m_model, m_animator3D);
    }

    // draw children
    for (auto&& child : children)
    {
        if (auto* renderable = dynamic_cast<Renderable*>(child.get()))
            renderable->RenderShadow(frustum);
    }
}
#pragma endregion

#pragma region player_controls
// public
// ----------------------------------------------------------------------
void Player::RequestMove(const glm::vec3 direction, bool isRunning)
{
    m_inputMoveDirection = direction;
    m_inputIsRunning = isRunning;
    
    if(m_actionState == ActionState::Attack1 || m_actionState == ActionState::Attack2 || m_actionState == ActionState::Attack3) return; // if performing attack, ignore move input

    m_moveDirection = direction;
    m_isRunning = isRunning;
}

void Player::RequestAttack()
{
    // if already acting, store input to action input buffer
    if(m_actionState != ActionState::None) {
        if(canCancelAction())
        {
            if(m_actionState == ActionState::Attack1) {
                m_inputActionState = ActionState::Attack2; // if currently performing first attack, buffer second attack input for combo
            }
            else if(m_actionState == ActionState::Attack2) {
                m_inputActionState = ActionState::Attack3; // if currently performing second attack, buffer third attack input for combo
            }
            else {
                m_inputActionState = ActionState::Attack1; // otherwise, buffer first attack input
            }
            m_actionInputBufferTime = ACTION_INPUT_BUFFER_TIME; // reset input buffer timer
        }
        return; 
    }

    startAction(ActionState::Attack1);
}

void Player::RequestRoll()
{
    // if already acting, store input to action input buffer
    if(m_actionState != ActionState::None) {
        if(canCancelAction())
        {
            m_inputActionState = ActionState::Roll; // store input action state to perform after current action finishes
            m_actionInputBufferTime = ACTION_INPUT_BUFFER_TIME;
        }
        return; 
    }

    startAction(ActionState::Roll);
}

ActionState Player::GetActionState() const
{
    return m_actionState;
}

MotionState Player::GetMotionState() const
{
    return m_motionState;
}

// private
// ----------------------------------------------------------------------
void Player::updateWorldHeight(const World &world)
{
    glm::vec3 localPos = transform.GetLocalPosition();
    localPos.y = world.GetWorldHeight(localPos.x, localPos.z);
    transform.SetLocalPosition(localPos);
}

void Player::startAction(ActionState action)
{
    m_animator3D.GetRootMotionDelta(); // reset root motion delta to prevent sudden position change when starting action

    m_actionState = action;
    m_actionTimer = 0.0f;
    m_moveDirection = glm::vec3(0.0f);
    m_isRunning = false;

    // if animation playing is forced, animation will play without blending from current animation
    if(action == ActionState::Attack1)
    {
        SetAnimation(m_animator3D.GetAnimation("Attack1"), true); // force play attack animation
    }
    else if(action == ActionState::Attack2)
    {
        SetAnimation(m_animator3D.GetAnimation("Attack2"), true); // force play attack animation
    }
    else if(action == ActionState::Attack3)
    {
        SetAnimation(m_animator3D.GetAnimation("Attack3"), true); // force play attack animation
    }
    else if(action == ActionState::Roll)
    {
        SetAnimation(m_animator3D.GetAnimation("Roll"), true); // force play roll animation
    }
}

void Player::updateAction(float dt)
{
    if(m_actionState == ActionState::None) return;

    // if there is buffered action input and can cancel current action, start buffered action
    if(m_inputActionState != ActionState::None && canCancelAction())
    {
        startAction(m_inputActionState);
        m_inputActionState = ActionState::None;
        return;
    }

    m_actionTimer += dt;
    if(m_actionState == ActionState::Attack1 || m_actionState == ActionState::Attack2 || m_actionState == ActionState::Attack3)
    {
        attack(dt);
    }
    else if(m_actionState == ActionState::Roll)
    {
        roll(dt);
    }

    m_actionInputBufferTime -= dt;
    if(m_actionInputBufferTime <= 0.0f)
    {
        m_inputActionState = ActionState::None; // clear buffered input if time window has passed
    }
}

void Player::transitionFromAction()
{
    // if attack animation finished, check if there is buffered action input to perform next
    m_actionState = m_inputActionState;
    m_inputActionState = ActionState::None;
    m_actionInputBufferTime = 0.0f;

    // restore input movement
    m_moveDirection = m_inputMoveDirection;
    m_isRunning = m_inputIsRunning;

    if(m_actionState != ActionState::None) startAction(m_actionState); // start next action if there is buffered input
}

bool Player::canCancelAction() const
{
    if(m_actionState == ActionState::None) return true;
    auto it = m_actionCancelTable.find(m_actionState);
    if(it == m_actionCancelTable.end()) return false; // if action state not found in cancel table, cannot cancel
    return m_actionTimer >= it->second; // can cancel if action timer has passed cancel time for current action state
}

void Player::transformFromActionAnimation(float dt)
{
    // apply position delta from root motion
    glm::vec3 rootMotionDelta = m_animator3D.GetRootMotionDelta();
    glm::vec3 localPosition = transform.GetLocalPosition();
    glm::vec3 localRotation = transform.GetLocalRotation();

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(localRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

    localPosition += glm::vec3(rotationMatrix * glm::vec4(rootMotionDelta, 1.0f)) * dt * 2.0f; // scale root motion delta for better movement feel
    transform.SetLocalPosition(localPosition);
}

void Player::updateMotion()
{
    if(m_actionState != ActionState::None) return;

    if(glm::length2(m_moveDirection) < 0.0001f)
    {
        m_motionState = MotionState::Idle;
        return;
    }

    m_motionState = m_isRunning ? MotionState::Run : MotionState::Walk;
}

void Player::updateAnimation()
{
    // if performing action, don't update animation based on motion state
    if(m_actionState != ActionState::None) return; 

    // update animation based on motion state
    if(m_motionState == MotionState::Idle)
    {
        SetAnimation(m_animator3D.GetAnimation("Idle"));
    }
    else if(m_motionState == MotionState::Walk)
    {
        SetAnimation(m_animator3D.GetAnimation("Walk"));
    }
    else if(m_motionState == MotionState::Run)
    {
        SetAnimation(m_animator3D.GetAnimation("Run"));
    }
}

void Player::move(Camera &camera, float dt)
{
    if(glm::length2(m_moveDirection) < 0.0001f) return;
    if(m_actionState == ActionState::Attack1 || m_actionState == ActionState::Attack2 || m_actionState == ActionState::Attack3) return;

    float velocity = Speed * dt * (m_isRunning ? 2.5f : 1.0f); // increase speed if running
    if(m_actionState == ActionState::Roll) velocity = 0.0f; // if rolling, only change direction

    // move direction
    glm::vec3 frontXZ = glm::normalize(glm::vec3(camera.cameraFront.x, 0.0f, camera.cameraFront.z));
    glm::vec3 rightXZ = glm::normalize(glm::cross(frontXZ, camera.worldUp));

    glm::vec3 moveDir = frontXZ * m_moveDirection.z + rightXZ * m_moveDirection.x; 
    moveDir = glm::normalize(moveDir);

    // set camera distance
    camera.targetDistance = NORMAL_DISTANCE;

    // rotate to move direction
    glm::vec3 localRot = transform.GetLocalRotation();
    float targetYaw = glm::degrees(atan2(moveDir.x, moveDir.z));
    // calculate angle difference
    float delta = NormalizeAngle(targetYaw - localRot.y);

    if (fabs(delta) > 140.0f)
        localRot.y = targetYaw;
    else
        localRot.y += delta * dt * 6.0f;
    localRot.y = NormalizeAngle(localRot.y);
    transform.SetLocalRotation(localRot);

    // move
    glm::vec3 localPosition = transform.GetLocalPosition();
    localPosition += moveDir * velocity;
    transform.SetLocalPosition(localPosition);
}

float NormalizeAngle(float angle)
{
    while(angle > 180.0f) angle -= 360.0f;
    while(angle < -180.0f) angle += 360.0f;
    return angle;
}

void Player::attack(float dt)
{
    const float attackDuration = 2.0f; // duration of the attack animation
    if(m_weapon != nullptr)
    {
        if(m_animator3D.IsAnimationFinished())
        {
            transitionFromAction();
            return;
        }

        transformFromActionAnimation(dt); // apply root motion from attack animation
    }
}

void Player::roll(float dt)
{
    const float rollDuration = 2.0f; // duration of the roll animation
    if(m_actionTimer >= rollDuration)
    {
        transitionFromAction();
        return;
    }

    transformFromActionAnimation(dt); // apply root motion from attack animation
}

#pragma endregion

#pragma region weapon_control
void Player::AttachWeapon(Weapon *weapon)
{
    m_weapon = weapon;
    weapon->SetOwner(this);
}

void Player::updateWeaponTransform()
{
    const int soket = m_sokets["RightHand"];
    const auto& bones = m_animator3D.GetGlobalBoneMatrices();

    // bone world matrix
    glm::mat4 boneModel = transform.GetModelMatrix() * bones[soket];
    // weapon
    glm::mat4 weaponLocalMat = m_weapon->transform.GetLocalMatrix();
    glm::mat4 weaponWorldMat = boneModel * weaponLocalMat;

    m_weapon->transform.SetModelMatrix(weaponWorldMat);
}
#pragma endregion