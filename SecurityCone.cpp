#include "SecurityCone.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Player.h"
#include "PlayerMove.h"
#include "Renderer.h"
#include <iostream>

SecurityCone::SecurityCone(Game* game, Actor* parent)
:Actor(game, parent)
{
    mTimer = 0.0f;
    mDead = false;
    mWhite = true;
    // Create components
    mMesh = new MeshComponent(this, true);
    mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cone.gpmesh"));
    SetPosition(Vector3(30,0,0));
    SetScale(3.0f);
    
    mCol = new CollisionComponent(this);
}

void SecurityCone::OnUpdate(float deltaTime)
{
    // Player is detected but not dead
    if (PlayerDetect() && !mDead) {
        mMesh->SetTextureIndex(1);
        mWhite = false;
        mTimer += deltaTime;
        if (mTimer >= 2.0f) {
            mMesh->SetTextureIndex(2);
            mDead = true;
            mTimer = 0.0f;
        }
    }
    // Player escaped camera
    else if (!PlayerDetect() && !mDead){
        mMesh->SetTextureIndex(0);
        mWhite = true;
        mTimer = 0.0f;
    }
    // Player is dead
    if (mDead) {
        mWhite = false;
        mTimer += deltaTime;
        if (mTimer >= 0.5f) {
            mGame->mPlayer->GetComponent<PlayerMove>()->Respawn();
            mTimer = 0.0f;
            mDead = false;
            mMesh->SetTextureIndex(0);
        }
    }

}

bool SecurityCone::PlayerDetect()
{
    // Vector from camera to player
    Vector3 camToPlayer = GetGame()->mPlayer->GetWorldPosition() - GetWorldPosition();
    Vector3 camToPlayerFwd = camToPlayer;
    camToPlayerFwd.Normalize();
    
    // Unsigned angle between the vector and forward vector
    float angle = acos( Vector3::Dot(camToPlayerFwd, GetWorldForward()) / camToPlayerFwd.Length() );
   
    float cameraLength = (GetWorldForward() * 300.0f).Length();
    // Player is inside
    if (angle <= (30.0f*Math::Pi)/180 && Vector3::Dot(camToPlayer, GetWorldForward()) <= cameraLength) {
        return true;
    }
    else {
        return false;
    }
}
