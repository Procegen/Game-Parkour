#include "LaserMine.h"
#include "MeshComponent.h"
#include "LaserComponent.h"
#include "CameraComponent.h"
#include "Renderer.h"
#include "Game.h"

LaserMine::LaserMine(Game* game, Actor* parent)
:Actor(game, parent)
{
    // Create components
    MeshComponent* mc = new MeshComponent(this);
    new LaserComponent(this);
    mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/LaserMine.gpmesh"));
}
