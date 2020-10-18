#pragma once
#include "Actor.h"

class Coin : public Actor
{
public:
    Coin(class Game* game, Actor* parent);
protected:
    void OnUpdate(float deltaTime) override;
private:
    class CollisionComponent* mCol;
};


