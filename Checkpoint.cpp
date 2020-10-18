#include "Checkpoint.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Renderer.h"

Checkpoint::Checkpoint(Game* game, Actor* parent)
:Actor(game, parent)
{
    mActive = true;
    // Create components
    mMesh = new MeshComponent(this);
    CollisionComponent* col = new CollisionComponent(this);
    
    col->SetSize(25.0f, 25.0f, 25.0f);
    mMesh->SetTextureIndex(1);
    mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Checkpoint.gpmesh"));
}

void Checkpoint::OnUpdate(float deltaTime)
{
    // Active checkpoint?
    if (mGame->mCheckpoints.front() == this) {
        mMesh->SetTextureIndex(0);
    }
    
    if (!mActive) {
        mGame->mCheckpoints.pop();
        SetState(ActorState::Destroy);
        if (!mLevelString.empty()) {
            mGame->SetNextLevel(mLevelString);
        }
    }
}
