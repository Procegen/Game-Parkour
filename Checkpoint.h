#pragma once
#include "Actor.h"
#include <string>

class Checkpoint : public Actor
{
public:
    Checkpoint(class Game* game, Actor* parent);
    void SetLevel(std::string level) { mLevelString = level; }
    void SetText(std::string text) { mText = text; }
    std::string GetText() const { return mText; }
    bool mActive;
protected:
    void OnUpdate(float deltaTime) override;
private:
    class MeshComponent* mMesh;
    std::string mLevelString;
    std::string mText;
};
