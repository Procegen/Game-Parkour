#include "Coin.h"
#include "Arrow.h"
#include "Checkpoint.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Player.h"
#include "Renderer.h"
#include "HUD.h"

Coin::Coin(Game* game, Actor* parent)
:Actor(game, parent)
{
    // Create components
    MeshComponent* mc = new MeshComponent(this);
    mCol = new CollisionComponent(this);
    
    mCol->SetSize(100.0f, 100.0f, 100.0f);
    mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Coin.gpmesh"));
}

void Coin::OnUpdate(float deltaTime)
{
    float rotSpeed = Math::Pi;
    mRotation += rotSpeed * deltaTime;
    
    CollisionComponent* playerCol = mGame->mPlayer->GetComponent<CollisionComponent>();
    if (mCol->Intersect(playerCol)) {
        Mix_Chunk *sample = GetGame()->GetSound("Assets/Sounds/Coin.wav");
        if(Mix_PlayChannel(-1, sample, 0)==-1) {
            printf("Mix_PlayChannel: %s\n",Mix_GetError());
        }
        mGame->mPlayer->GetComponent<HUD>()->UpdateCoin();
        SetState(ActorState::Destroy);
    }
}
