#pragma once
#include "Actor.h"

class Player : public Actor
{
public:
    Player(class Game* game, Actor* parent);
    void SetRespawnPos(Vector3 respawnPosition) { mRespawnPos = respawnPosition; }
    Vector3 GetRespawnPos() const { return mRespawnPos; }
private:
    Vector3 mRespawnPos;
};
