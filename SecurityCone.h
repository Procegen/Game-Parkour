#pragma once
#include "Actor.h"

class SecurityCone : public Actor
{
public:
    SecurityCone(class Game* game, Actor* parent);
    bool PlayerDetect();
    bool mWhite;
protected:
    void OnUpdate(float deltaTime) override;
private:
    class CollisionComponent* mCol;
    class MeshComponent* mMesh;
    float mTimer;
    bool mDead;
};


