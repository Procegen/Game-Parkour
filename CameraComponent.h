#include "Component.h"
#include "Math.h"

class CameraComponent : public Component
{
public:
    CameraComponent(class Actor* owner);
    void Update(float deltaTime) override;
    void SetPitchSpeed(float pitchSpeed) { mPitchSpeed = pitchSpeed; }
    float GetPitchSpeed() { return mPitchSpeed; }
    void SetRollSpeed(float rollSpeed) { mRollSpeed = rollSpeed; }
    float GetRollSpeed() { return mRollSpeed; }
    void SetWallRun(bool wallRun) { mWallRun = wallRun; }
    bool GetWallRun() { return mWallRun; }
    void SetResetRoll(bool resetRoll) { mResetRoll = resetRoll; }
    bool GetResetRoll() { return mResetRoll; }
    bool mSideX;
private:
    bool mResetRoll;
    bool mWallRun;
    float mHorDist;
    float mTargetOffest;
    float mPitchAngle;
    float mPitchSpeed;
    float mRollAngle;
    float mRollSpeed;
    Vector3 mCamPos;
};
