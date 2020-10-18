#pragma once
#include "Actor.h"

class Block : public Actor
{
public:
    Block(class Game* game, Actor* parent);
    ~Block();
    void SetMirror(bool mirror) { mMirror = mirror; }
    bool GetMirror() const { return mMirror; }
    void SetRotate(bool rotate) { mRotate = rotate; }
    bool GetRotate() const {return mRotate; }
protected:
    void OnUpdate(float deltaTime) override;
private:
    bool mMirror;
    bool mRotate;
};

