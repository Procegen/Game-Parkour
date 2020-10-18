#include "Player.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "CameraComponent.h"
#include "PlayerMove.h"
#include "Game.h"
#include "Renderer.h"
#include "HUD.h"

Player::Player(Game* game, Actor* parent)
:Actor(game, parent)
{
    // Create components
    new MeshComponent(this);
    new PlayerMove(this);
    new CameraComponent(this);
    new HUD(this);
    CollisionComponent* col = new CollisionComponent(this);
    col->SetSize(50.0f, 175.0f, 50.0f);
}

