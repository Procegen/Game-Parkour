#include "Block.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "CameraComponent.h"
#include "PlayerMove.h"
#include "Game.h"
#include "Renderer.h"

Block::Block(Game* game, Actor* parent)
:Actor(game, parent)
{
    // Create components
    MeshComponent* mc = new MeshComponent(this);
    CollisionComponent* col = new CollisionComponent(this);
    
    col->SetSize(1, 1, 1);
    mScale = 64;
    mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
    mGame->AddBlock(this);
    mMirror = false;
}
Block::~Block()
{
    mGame->RemoveBlock(this);
}

void Block::OnUpdate(float deltaTime)
{
    if (mRotate) {
        float rotSpeed = Math::Pi/4;
        float rot = GetRotation() + rotSpeed * deltaTime;
        SetRotation(rot);
    }
}
