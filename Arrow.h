#pragma once
#include "Actor.h"

class Arrow : public Actor
{
public:
    Arrow(class Game* game, Actor* parent);
protected:
    void OnUpdate(float deltaTime) override;
};

