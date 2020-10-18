#include "PlayerMove.h"
#include "Player.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "SDL/SDL_mixer.h"
#include "CollisionComponent.h"
#include "CameraComponent.h"
#include "Math.h"
#include "Player.h"
#include "Checkpoint.h"
#include "SecurityCamera.h"
#include "HUD.h"
#include <random>
#include <string>
#include <iostream>

PlayerMove::PlayerMove(class Actor* owner)
:MoveComponent(owner)
{
    ChangeState(MoveState::Falling);
    mZSpeed = 0.0f;
    mJumped = false;
    mJumpPlayed = false;
    mMass = 1.0f;
    mGravity = Vector3(0.0f, 0.0f, -980.0f);
    mJumpForce = Vector3(0.0f, 0.0f, 35000.0f);
    mClimbForce = Vector3(0.0f, 0.0f, 1800.0f);
    mWallRunForce = Vector3(0.0f, 0.0f, 1200.f);
    mWallClimbTimer = 0.0f;
    mWallRunTimer = 0.0f;
    Mix_Chunk *run = mOwner->GetGame()->GetSound("Assets/Sounds/Running.wav");
    mRunningSFX = Mix_PlayChannel(-1, run, -1);
    Mix_Pause(mRunningSFX);
}

PlayerMove::~PlayerMove()
{
    Mix_HaltChannel(mRunningSFX);
}

void PlayerMove::Update(float deltaTime)
{
    if (mCurrentState == Jump) {
        UpdateJump(deltaTime);
        Mix_Pause(mRunningSFX);
    }
    else if (mCurrentState == Falling) {
        UpdateFalling(deltaTime);
        Mix_Pause(mRunningSFX);
    }
    else if (mCurrentState == OnGround) {
        UpdateOnGround(deltaTime);
        if (mVelocity.Length() > 50.0f) {
            Mix_Resume(mRunningSFX);
        }
    }
    else if (mCurrentState == WallClimb) {
        UpdateWallClimb(deltaTime);
        Mix_Resume(mRunningSFX);
    }
    else {
        UpdateWallRun(deltaTime);
        Mix_Resume(mRunningSFX);
    }
    // Respawn?
    if (mOwner->GetPosition().z < -750) {
        Respawn();
    }
    
    if (!(mOwner->GetGame()->mCheckpoints.empty())) {
        // Collide with checkpoint?
        CollisionComponent* playerCol = mOwner->GetComponent<CollisionComponent>();
        CollisionComponent* checkpointCol = mOwner->GetGame()->mCheckpoints.front()->GetComponent<CollisionComponent>();
        if(playerCol->Intersect(checkpointCol)) {
            mOwner->GetGame()->UpdateCheckpoint();
            mOwner->GetGame()->mPlayer->SetRespawnPos(mOwner->GetGame()->mCheckpoints.front()->GetPosition());
            mOwner->GetGame()->mCheckpoints.front()->mActive = false;
            Mix_Chunk *sample = mOwner->GetGame()->GetSound("Assets/Sounds/Checkpoint.wav");
            if(Mix_PlayChannel(-1, sample, 0)==-1) {
                printf("Mix_PlayChannel: %s\n",Mix_GetError());
            }
        }
    }
}

void PlayerMove::ProcessInput(const Uint8 *keyState)
{
    // W / S
    if (keyState[SDL_SCANCODE_W] && !keyState[SDL_SCANCODE_S]) {
        AddForce(mOwner->GetForward()*700.0f);
    }
    else if (!keyState[SDL_SCANCODE_W] && keyState[SDL_SCANCODE_S]) {
        AddForce(mOwner->GetForward()*-700.0f);
    }
    
    // A / D
    if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D]) {
        AddForce(mOwner->GetRight()*-700.0f);
    }
    else if (!keyState[SDL_SCANCODE_A] && keyState[SDL_SCANCODE_D]) {
        AddForce(mOwner->GetRight()*700.0f);
    }
    
    // Jump
    if (keyState[SDL_SCANCODE_SPACE] && !mJumped && mCurrentState == MoveState::OnGround) {
        mJumped = true;
        AddForce(mJumpForce);
        ChangeState(MoveState::Jump);
        Mix_Chunk *sample = mOwner->GetGame()->GetSound("Assets/Sounds/Jump.wav");
        if(Mix_PlayChannel(-1, sample, 0)==-1) {
            printf("Mix_PlayChannel: %s\n",Mix_GetError());
        }
    }
    else if (!keyState[SDL_SCANCODE_SPACE] && mJumped) {
        mJumped = false;
    }
    
    // Rotation
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    float angularSpeed =  (float)x / 500.0f * Math::Pi * 10.0f;
    float pitchSpeed =  (float)y / 500.0f * Math::Pi * 10.0f;
    SetAngularSpeed(angularSpeed);
    mOwner->GetComponent<CameraComponent>()->SetPitchSpeed(pitchSpeed);
}

void PlayerMove::UpdateJump(float deltaTime) {
    mJumpPlayed = false;
    AddForce(mGravity);
    PhysicsUpdate(deltaTime);
    
    // Loop through blocks
    std::vector<Actor*> blocks = mOwner->GetGame()->GetBlocks();
    CollisionComponent* playerCol = mOwner->GetComponent<CollisionComponent>();
    for (auto i : blocks) {
        CollisionComponent* blockCol = i->GetComponent<CollisionComponent>();
        CollSide collSide = FixCollision(playerCol, blockCol);
        if (collSide == CollSide::Bottom) {
            mVelocity.z = 0.0f;
        }
        if (mVelocity.z <= 0.0f) {
            ChangeState(MoveState::Falling);
        }
        if (collSide != CollSide::None) {
            if (CanWallClimb(collSide)) {
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
            else if (CanWallRun(collSide)){
                std::cout << "Wall run" << std::endl;
                ChangeState(WallRun);
                mWallRunTimer = 0.0f;
                return;
            }
        }
    }
}
void PlayerMove::UpdateFalling(float deltaTime) {
    AddForce(mGravity);
    PhysicsUpdate(deltaTime);
    
    // Loop through blocks
    std::vector<Actor*> blocks = mOwner->GetGame()->GetBlocks();
    CollisionComponent* playerCol = mOwner->GetComponent<CollisionComponent>();
    for (auto i : blocks) {
        CollisionComponent* blockCol = i->GetComponent<CollisionComponent>();
        CollSide collSide = FixCollision(playerCol, blockCol);
        if (collSide == CollSide::Top) {
            if (!mJumpPlayed) {
                Mix_Chunk *sample = mOwner->GetGame()->GetSound("Assets/Sounds/Land.wav");
                if(Mix_PlayChannel(-1, sample, 0)==-1) {
                    printf("Mix_PlayChannel: %s\n",Mix_GetError());
                }
                mJumpPlayed = true;
            }
            mVelocity.z = 0.0f;
            ChangeState(MoveState::OnGround);
        }
    }
}

void PlayerMove::UpdateOnGround(float deltaTime) {
    PhysicsUpdate(deltaTime);
    
    // Loop through blocks
    std::vector<Actor*> blocks = mOwner->GetGame()->GetBlocks();
    CollisionComponent* playerCol = mOwner->GetComponent<CollisionComponent>();
    for (auto i : blocks) {
        CollisionComponent* blockCol = i->GetComponent<CollisionComponent>();
        CollSide collSide = FixCollision(playerCol, blockCol);
        if (collSide == CollSide::Top) {
            mZSpeed = 0.0f;
            ChangeState(MoveState::OnGround);
        }
        else {
            ChangeState(MoveState::Falling);
        }
        if (collSide != CollSide::None) {
            if (CanWallClimb(collSide)) {
                ChangeState(WallClimb);
                mWallClimbTimer = 0.0f;
                return;
            }
        }
    }
}

void PlayerMove::UpdateWallClimb(float deltaTime) {
    mWallClimbTimer += deltaTime;
    AddForce(mGravity);
    if (mWallClimbTimer < 0.4f) {
        AddForce(mClimbForce);
    }
    PhysicsUpdate(deltaTime);
    
    // Loop through blocks
    std::vector<Actor*> blocks = mOwner->GetGame()->GetBlocks();
    CollisionComponent* playerCol = mOwner->GetComponent<CollisionComponent>();
    bool collided = false;
    for (auto i : blocks) {
        CollisionComponent* blockCol = i->GetComponent<CollisionComponent>();
        CollSide collSide = FixCollision(playerCol, blockCol);
        if (collSide != CollSide::None && collSide != CollSide::Top && collSide != CollSide::Bottom) {
            collided = true;
        }
        if (mVelocity.z <= 0.0f) {
            mVelocity.z = 0.0f;
            ChangeState(MoveState::Falling);
        }
        if (i == blocks.back() && !collided) {
            mVelocity.z = 0.0f;
            ChangeState(MoveState::Falling);
        }
    }
}

void PlayerMove::UpdateWallRun(float deltaTime) {
    mWallRunTimer += deltaTime;
    AddForce(mGravity);
    if (mWallRunTimer < 0.4f) {
        AddForce(mWallRunForce);
    }
    PhysicsUpdate(deltaTime);
    
    // Loop through blocks
    std::vector<Actor*> blocks = mOwner->GetGame()->GetBlocks();
    CollisionComponent* playerCol = mOwner->GetComponent<CollisionComponent>();
    bool collided = false;
    for (auto i : blocks) {
        CollisionComponent* blockCol = i->GetComponent<CollisionComponent>();
        CollSide collSide = FixCollision(playerCol, blockCol);
        if (collSide != CollSide::None && collSide != CollSide::Top && collSide != CollSide::Bottom) {
            collided = true;
        }
        if (mVelocity.z <= 0.0f) {
            mVelocity.z = 0.0f;
            ChangeState(MoveState::Falling);
            mOwner->GetComponent<CameraComponent>()->SetRollSpeed(0.0f);
            mOwner->GetComponent<CameraComponent>()->SetResetRoll(true);
        }
    }
}

bool PlayerMove::CanWallClimb(CollSide collSide) {
    float const CLIMB_THRESH = 0.2;
    float const LENGTH_THRESH = 350.0f;
    Vector2 xyVelocity = Vector2(mVelocity.x, mVelocity.y);
    Vector2 xyVelForward = Vector2(mVelocity.x, mVelocity.y);
    xyVelForward.Normalize();

    if (xyVelocity.Length() > LENGTH_THRESH) {
        if (collSide == CollSide::SideX1) {
            if (mOwner->GetForward().x >= 1.0f-CLIMB_THRESH && mOwner->GetForward().x <= 1.0f+CLIMB_THRESH && xyVelForward.x >= 1.0f-CLIMB_THRESH && xyVelForward.x <= 1.0f+CLIMB_THRESH && xyVelocity.Length() > LENGTH_THRESH) {
                return true;
            }
        }
        else if (collSide == CollSide::SideX2) {
            if (mOwner->GetForward().x >= -1.0f-CLIMB_THRESH && mOwner->GetForward().x <= -1.0f+CLIMB_THRESH && xyVelForward.x >= -1.0f-CLIMB_THRESH && xyVelForward.x <= -1.0f+CLIMB_THRESH && xyVelocity.Length() > LENGTH_THRESH) {
                return true;
            }
        }
        else if (collSide == CollSide::SideY1) {
            if (mOwner->GetForward().y >= 1.0f-CLIMB_THRESH && mOwner->GetForward().y <= 1.0f+CLIMB_THRESH && xyVelForward.y >= 1.0f-CLIMB_THRESH && xyVelForward.y <= 1.0f+CLIMB_THRESH && xyVelocity.Length() > LENGTH_THRESH) {
                return true;
            }
        }
        else if (collSide == CollSide::SideY2) {
            if (mOwner->GetForward().y >= -1.0f-CLIMB_THRESH && mOwner->GetForward().y <= -1.0f+CLIMB_THRESH && xyVelForward.y >= -1.0f-CLIMB_THRESH && xyVelForward.y <= -1.0f+CLIMB_THRESH && xyVelocity.Length() > LENGTH_THRESH) {
                return true;
            }
        }
    }
    
    return false;
}

bool PlayerMove::CanWallRun(CollSide collSide) {
    float const CLIMB_THRESH = 0.15;
    float const LENGTH_THRESH = 350.0f;
    float rollSpeed = 0.0f;
    bool movingForward = false;
    Vector2 xyVelocity = Vector2(mVelocity.x, mVelocity.y);
    Vector2 ownerForward = Vector2(mOwner->GetForward().x, mOwner->GetForward().y);
    Vector2 xyVelForward = Vector2(mVelocity.x, mVelocity.y);
    xyVelForward.Normalize();
    
    if (abs(xyVelForward.x-ownerForward.x) <= CLIMB_THRESH && abs(xyVelForward.x-ownerForward.x) >= -CLIMB_THRESH && abs(xyVelForward.y-ownerForward.y) <= CLIMB_THRESH && abs(xyVelForward.y-ownerForward.y) >= -CLIMB_THRESH) {
        movingForward = true;
    }
       
    float d = xyVelForward.Dot(xyVelForward, ownerForward);
    
    if (xyVelocity.Length() > LENGTH_THRESH && d >= 1-CLIMB_THRESH && d <= 1+CLIMB_THRESH && movingForward) {
        // Check collision side
        if (collSide == CollSide::SideX1) {
            std::cout << "x1" << std::endl;
            rollSpeed = -2.0f;
            mOwner->GetComponent<CameraComponent>()->SetRollSpeed(rollSpeed);
            mOwner->GetComponent<CameraComponent>()->mSideX = true;
            return true;
        }
        else if (collSide == CollSide::SideX2) {
            std::cout << "x2" << std::endl;
            rollSpeed = 2.0f;
            mOwner->GetComponent<CameraComponent>()->SetRollSpeed(rollSpeed);
            mOwner->GetComponent<CameraComponent>()->mSideX = true;
            return true;
        }
        else if (collSide == CollSide::SideY1) {
            std::cout << "y1" << std::endl;
            rollSpeed = 2.0f;
            mOwner->GetComponent<CameraComponent>()->SetRollSpeed(rollSpeed);
            mOwner->GetComponent<CameraComponent>()->mSideX = false;
            return true;
        }
        else if (collSide == CollSide::SideY2) {
            std::cout << "y2" << std::endl;
            rollSpeed = -2.0f;
            mOwner->GetComponent<CameraComponent>()->SetRollSpeed(rollSpeed);
            mOwner->GetComponent<CameraComponent>()->mSideX = false;
            return true;
        }
    }
    
    return false;
}

CollSide PlayerMove::FixCollision(class CollisionComponent* self, class CollisionComponent* block) {
    Vector3 offset = Vector3::Zero;
    CollSide collSide = self->GetMinOverlap(block, offset);
    if ( collSide != CollSide::None) {
        Vector3 pos = mOwner->GetPosition();
        pos += offset;
        mOwner->SetPosition(pos);
    }
    // Add normal force
    if (collSide == CollSide::SideX1) {
        AddForce(Vector3(-700.0f, 0.0f, 0.0f));
    }
    else if (collSide == CollSide::SideX2) {
        AddForce(Vector3(700.0f, 0.0f, 0.0f));
    }
    else if (collSide == CollSide::SideY1) {
        AddForce(Vector3(0.0f, -700.0f, 0.0f));
    }
    else if (collSide == CollSide::SideY2) {
        AddForce(Vector3(0.0f, 700.0f, 0.0f));
    }
    return collSide;
}

void PlayerMove::PhysicsUpdate(float deltaTime) {
    mAcceleration = mPendingForces * (1.0f / mMass);
    mVelocity += mAcceleration * deltaTime;
    FixXYVelocity();
    
    Vector3 pos = mOwner->GetPosition();
    pos += mVelocity * deltaTime;
    mOwner->SetPosition(pos);
    
    float rot = mOwner->GetRotation();
    rot += GetAngularSpeed() * deltaTime;
    mOwner->SetRotation(rot);
    
    mPendingForces = Vector3::Zero;
}
void PlayerMove::AddForce(const Vector3& force) {
    mPendingForces += force;
}

void PlayerMove::FixXYVelocity() {
    Vector2 xyVelocity = Vector2(mVelocity.x, mVelocity.y);
    float length = xyVelocity.Length();
    float const MAX_SPEED = 400.0f;
    float const BRAK_FACTOR = 0.7f;
    
    // Limit speed
    if (length > MAX_SPEED) {
        xyVelocity.Normalize();
        xyVelocity = xyVelocity * MAX_SPEED;
    }
    // Apply braking
    if (mCurrentState == OnGround || mCurrentState == WallClimb) {
        if (Math::NearZero(mAcceleration.x)) {
            xyVelocity.x *= BRAK_FACTOR;
        }
        if (Math::NearZero(mAcceleration.y)) {
            xyVelocity.y *= BRAK_FACTOR;
        }
        if (mAcceleration.x * xyVelocity.x < 0) {
            xyVelocity.x *= BRAK_FACTOR;
        }
        if (mAcceleration.y * xyVelocity.y < 0) {
            xyVelocity.y *= BRAK_FACTOR;
        }
    }
    
    mVelocity.x = xyVelocity.x;
    mVelocity.y = xyVelocity.y;
}

void PlayerMove::Respawn()
{
    mOwner->SetPosition(mOwner->GetGame()->mPlayer->GetRespawnPos());
    mOwner->SetRotation(0.0f);
    mVelocity = Vector3::Zero;
    mPendingForces = Vector3::Zero;
    mCurrentState = MoveState::Falling;
}
