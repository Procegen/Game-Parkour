#include "Arrow.h"
#include "Checkpoint.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Player.h"
#include "Renderer.h"

Arrow::Arrow(Game* game, Actor* parent)
:Actor(game, parent)
{
    // Create components
    MeshComponent* mc = new MeshComponent(this);
    
    mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Arrow.gpmesh"));
    SetScale(0.15f);
}

void Arrow::OnUpdate(float deltaTime)
{
    if (!mGame->mCheckpoints.empty()) {
        // vector from the player position to the active checkpoint
        Vector3 playerToCheck = mGame->mCheckpoints.front()->GetPosition() - mGame->mPlayer->GetPosition();
        playerToCheck.Normalize();
        
        float dot = Vector3::Dot(Vector3::UnitX, playerToCheck);
        float angle = acos(dot);
        Vector3 axis = Vector3::Cross(Vector3::UnitX, playerToCheck);
        axis.Normalize();
        
        
        if (dot == 1) {
            mQuatRotation = Quaternion::Identity;
        }
        else if (dot == -1) {
            mQuatRotation = Quaternion(Vector3::UnitZ, Math::Pi);
        }
        else {
            mQuatRotation = Quaternion(axis, angle);
        }
    }
    else {
        mQuatRotation = Quaternion::Identity;
    }
    
    Vector3 arrowPos = mGame->GetRenderer()->Unproject(Vector3(0.0f, 250.0f, 0.1f));
    SetPosition(arrowPos);
}
