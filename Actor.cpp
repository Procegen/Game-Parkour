#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>
#include <iostream>

Actor::Actor(Game* game, Actor* parent)
	:mGame(game)
	,mState(ActorState::Active)
	,mPosition(Vector3::Zero)
	,mScale(1.0f)
	,mRotation(0.0f)
{
    // Add actor
    if (parent == nullptr) {
        mGame->AddActor(this);
    }
    else {
        parent->AddChild(this);
    }
    mParent = parent;
}

Actor::~Actor()
{
    // Delete children
    while (!mChildren.empty()) {
        delete mChildren.back();
    }
    
    if (mParent == nullptr) {
        // Remove actor
        mGame->RemoveActor(this);
    }
    else {
        mParent->RemoveChild(this);
    }
    
    // Delete each component and clear vector
    for (unsigned int i = 0; i < mComponents.size(); i++) {
        delete mComponents[i];
    }
    mComponents.clear();
}

void Actor::Update(float deltaTime)
{
    CalcWorldTransform();
    // Update if the actor state is Active.
    if (Actor::GetState() == ActorState::Active) {
        // Call update on all its components
        for (unsigned int i = 0; i < mComponents.size(); i++) {
            mComponents[i]->Update(deltaTime);
        }
        OnUpdate(deltaTime);
    }
    CalcWorldTransform();
    
    for (auto i : mChildren) {
        i->Update(deltaTime);
    }
}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState)
{
	// Update if the actor state is Active
    if (Actor::GetState() == ActorState::Active) {
        // Call ProcessInput on all its components
        for (unsigned int i = 0; i < mComponents.size(); i++) {
            mComponents[i]->ProcessInput(keyState);
        }
        OnProcessInput(keyState);
    }
}

void Actor::OnProcessInput(const Uint8* keyState)
{
}

void Actor::AddComponent(Component* c)
{
	mComponents.emplace_back(c);
	std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
		return a->GetUpdateOrder() < b->GetUpdateOrder();
	});
}

// returns the forward direction vector of the actor
Vector3 Actor::GetForward()
{
    float vectX = cos(mRotation);
    float vectY = sin(mRotation);
    float vectZ = 0.0f;
    Vector3 forwardVect;
    forwardVect.Set(vectX, vectY, vectZ);
    return forwardVect;
}

// returns the right direction vector of the actor
Vector3 Actor::GetRight()
{
    float vectX = cos(mRotation + Math::PiOver2);
    float vectY = sin(mRotation + Math::PiOver2);
    float vectZ = 0.0f;
    Vector3 rightVect;
    rightVect.Set(vectX, vectY, vectZ);
    return rightVect;
}

Vector3 Actor::GetQuatForward()
{
    Vector3 retVal = Vector3::Transform(Vector3::UnitX, mQuatRotation);
    retVal.Normalize();
    return retVal;
}

void Actor::CalcWorldTransform()
{
    // Set mWorldTransform - worldMatrix = scaleMatrix * rotationMatrix * positionMatrix
    Matrix4 scale = Matrix4::CreateScale(mScale);
    Matrix4 rotation = Matrix4::CreateRotationZ(mRotation);
    Matrix4 position = Matrix4::CreateTranslation(mPosition);
    Matrix4 quatRotation = Matrix4::CreateFromQuaternion(mQuatRotation);
    
    mWorldTransform = scale * rotation * quatRotation * position;
    
    // is there a parent?
    if (mParent != nullptr) {
        if (mInheritScale) {
            mWorldTransform = mWorldTransform * mParent->mWorldTransform;
        }
        else {
            mWorldTransform = mWorldTransform * mParent->CalcWorldRotTrans();
        }
    }
    
    
}

Matrix4 Actor::CalcWorldRotTrans()
{
    // Set mWorldTransform - worldMatrix = scaleMatrix * rotationMatrix * positionMatrix
    Matrix4 rotation = Matrix4::CreateRotationZ(mRotation);
    Matrix4 position = Matrix4::CreateTranslation(mPosition);
    Matrix4 quatRotation = Matrix4::CreateFromQuaternion(mQuatRotation);

    Matrix4 retVal = rotation * quatRotation * position;
    
    // is there a parent?
    if (mParent != nullptr) {
        retVal = retVal * mParent->CalcWorldRotTrans();
    }
    
    return retVal;
}

void Actor::AddChild(Actor *child)
{
    mChildren.emplace_back(child);
}

void Actor::RemoveChild(Actor *child)
{
    auto iter = std::find(mChildren.begin(), mChildren.end(), child);
    if (iter != mChildren.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        auto iter2 = mChildren.end() - 1;
        std::iter_swap(iter, iter2);
        mChildren.pop_back();
    }
}
