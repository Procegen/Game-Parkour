#pragma once
#include "MoveComponent.h"
#include "Math.h"
#include "CollisionComponent.h"

class PlayerMove : public MoveComponent
{
public:
    enum MoveState
    {
        OnGround,
        Jump,
        Falling,
        WallClimb,
        WallRun
    };
    PlayerMove(class Actor* owner);
    ~PlayerMove() override;
    void Update(float deltaTime) override;
    void ProcessInput(const Uint8* keyState) override;
    void ChangeState(MoveState state) {mCurrentState = state;}
    void Respawn();
protected:
    void UpdateOnGround(float deltaTime);
    void UpdateJump(float deltaTime);
    void UpdateFalling(float deltaTime);
    void UpdateWallClimb(float deltaTime);
    void UpdateWallRun(float deltaTime);
    bool CanWallClimb(CollSide collSide);
    bool CanWallRun(CollSide collSide);
    CollSide FixCollision(class CollisionComponent* self, class CollisionComponent* block);
    void PhysicsUpdate(float deltaTime);
    void AddForce(const Vector3& force);
    void FixXYVelocity();
private:
    Vector3 mPos;
    MoveState mCurrentState;
    float mZSpeed;
    const float GRAVITY = -980.0f;
    bool mJumped;
    bool mJumpPlayed;
    Vector3 mVelocity;
    Vector3 mAcceleration;
    Vector3 mPendingForces;
    Vector3 mGravity;
    Vector3 mJumpForce;
    Vector3 mClimbForce;
    Vector3 mWallRunForce;
    float mWallRunTimer;
    float mMass;
    float mWallClimbTimer;
    int mRunningSFX;
};

