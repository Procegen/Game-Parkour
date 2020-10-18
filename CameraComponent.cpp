#include "CameraComponent.h"
#include "Math.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "PlayerMove.h"
#include <iostream>

CameraComponent::CameraComponent(class Actor* owner)
:Component(owner)
{
    mHorDist = 60.0f;
    mTargetOffest = 50.0f;
    mCamPos = Vector3::Zero;
    mPitchAngle = 0.0f;
    mPitchSpeed = 0.0f;
    mRollAngle = 0.0f;
    mRollSpeed = 0.0f;
    mResetRoll = false;
    mSideX = false;
}

void CameraComponent::Update(float deltaTime)
{
    mPitchAngle += mPitchSpeed * deltaTime;
    mPitchAngle = Math::Clamp(mPitchAngle, -Math::PiOver2/2, Math::PiOver2/2);

//    std::cout << "mResetRoll: " + std::to_string(mResetRoll) << std::endl;
//    std::cout << mRollAngle << std::endl;
    
    if (mResetRoll) {
        mRollAngle = Math::Lerp(mRollAngle, 0.0f, deltaTime*5);
        if (Math::NearZero(abs(mRollAngle))) {
            mRollAngle = 0.0f;
            mResetRoll = false;
        }
    }
    else {
        mRollAngle += mRollSpeed * deltaTime;
    }
    mRollAngle = Math::Clamp(mRollAngle, -Math::PiOver2/2, Math::PiOver2/2);
    
    Matrix4 roll;
    if (mSideX) {
        roll = Matrix4::CreateRotationY(mRollAngle);
    }
    else {
        roll = Matrix4::CreateRotationX(mRollAngle);
    }
    Matrix4 pitch = Matrix4::CreateRotationY(mPitchAngle);
    Matrix4 yaw = Matrix4::CreateRotationZ(mOwner->GetRotation());

    Matrix4 rotation = pitch * yaw;
    Vector3 forward = Vector3::Transform(Vector3(1,0,0), rotation);
    Vector3 up = Vector3::Transform(Vector3::UnitZ, roll);
    
    Vector3 targetPos = mOwner->GetPosition() + (forward * mTargetOffest);
    Matrix4 view = Matrix4::CreateLookAt(mOwner->GetPosition(), targetPos, up);
    mOwner->GetGame()->GetRenderer()->SetViewMatrix(view);
}

