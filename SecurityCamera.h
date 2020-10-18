#pragma once
#include "Actor.h"

class SecurityCamera : public Actor
{
public:
    SecurityCamera(class Game* game, Actor* parent);
    ~SecurityCamera() override;
    Quaternion GetStartQ() const { return mStartQ; }
    Quaternion GetEndQ() const { return mEndQ; }
    float GetInterpTime() const { return mInterpTime; }
    float GetPauseTime() const { return mPauseTime; }
    void SetStartQ(Quaternion startQ) { mStartQ = startQ; }
    void SetEndQ(Quaternion endQ) { mEndQ = endQ; }
    void SetInterpTime(float interpTime) { mInterpTime = interpTime;}
    void SetPauseTime(float pauseTime) { mPauseTime = pauseTime; }
    void AdjustVolume(int channel);
    int mSoundRotate;
protected:
    void OnUpdate(float deltaTime) override;
private:
    class SecurityCone* mCone;
    class CollisionComponent* mCol;
    Quaternion mStartQ;
    Quaternion mEndQ;
    float mInterpTime;
    float mPauseTime;
    float mTimer;
    float mPauseTimer;
    bool mReverse;
    bool mPause;
    bool mHalted;
    bool mDetectPlaying;
    int mDetectSound;
};


