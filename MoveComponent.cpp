#include "MoveComponent.h"
#include "Actor.h"
#include <iostream>

MoveComponent::MoveComponent(class Actor* owner)
:Component(owner, 50)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
{
	
}

void MoveComponent::Update(float deltaTime)
{
    //  update the owning actor’s rotation
    float rotSpeed = mOwner->GetRotation();
    rotSpeed += mAngularSpeed * deltaTime;
    mOwner->SetRotation(rotSpeed);
    //  update the owning actor’s position
    Vector3 position = mOwner->GetPosition();
    position += mOwner->GetForward() * mForwardSpeed * deltaTime;
    position += mOwner->GetRight() * mStrafeSpeed * deltaTime;
    mOwner->SetPosition(position);
}
