#include "SecurityCamera.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Player.h"
#include "PlayerMove.h"
#include "SecurityCone.h"
#include <iostream>

SecurityCamera::SecurityCamera(Game* game, Actor* parent)
:Actor(game, parent)
{
    mHalted = false;
    mTimer = 0.0f;
    mReverse = false;
    mPause = false;
    mDetectPlaying = false;
    mSoundRotate = -1;
    // Create components
    MeshComponent* mc = new MeshComponent(this);
    mc->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Camera.gpmesh"));
    
    mCone = new SecurityCone(game, this);
    
    game->AddCamera(this);
}

SecurityCamera::~SecurityCamera()
{
    if (mSoundRotate != -1) {
        Mix_HaltChannel(mSoundRotate);
    }
    mGame->RemoveCamera(this);
}

void SecurityCamera::OnUpdate(float deltaTime)
{
    // Pause if player is detected
    if (!mCone->mWhite) {
        mPause = true;
        if (!mHalted) {
            
            if(mSoundRotate != -1)
            {
                Mix_HaltChannel(mSoundRotate);
                mSoundRotate = -1;
            }
            
            mHalted = true;
            Mix_Chunk *sound = GetGame()->GetSound("Assets/Sounds/SecurityDetected.wav");
            mDetectSound = Mix_PlayChannel(Mix_GroupAvailable(1), sound, 0);
            mDetectPlaying = true;
            
            Mix_Chunk *motor = GetGame()->GetSound("Assets/Sounds/CameraMotorStop.wav");
            int motorStop = Mix_PlayChannel(Mix_GroupAvailable(1), motor, 0);
            AdjustVolume(motorStop);
        }
    }
    // Only halt when detect sound is playing
    else if (mDetectPlaying){
        if(mDetectSound != -1)
        {
            Mix_HaltChannel(mDetectSound);
            mDetectSound = -1;
        }
        mDetectPlaying = false;
    }
    
    // Paused
    if (mPause) {
        // Play motorstop if camera just paused
        if (mPauseTimer == 0.0f) {
            Mix_Chunk *sound = GetGame()->GetSound("Assets/Sounds/CameraMotorStop.wav");
            int motorStop = Mix_PlayChannel(Mix_GroupAvailable(1), sound, 0);
            AdjustVolume(motorStop);
        }
        mPauseTimer += deltaTime;
        
        // Start moving again
        if (mCone->mWhite && mPauseTimer >= mPauseTime) {
            mPauseTimer = 0.0f;
            mPause = false;
            
            if (mReverse) {
                mReverse = false;
                if (mTimer <= 0.0f) {
                    mTimer = 0.0f;
                }
            }
            else {
                mReverse = true;
                if (mTimer >= 1.0f) {
                    mTimer = 1.0f;
                }
            }
            mHalted = false;
            Mix_Chunk *sound = GetGame()->GetSound("Assets/Sounds/CameraMotor.wav");
            mSoundRotate = Mix_PlayChannel(Mix_GroupAvailable(1), sound, 0);
            AdjustVolume(mSoundRotate);
        }
    }
    else {
        // Reverse rotation
        if (!mReverse) {
            mTimer += 1/mInterpTime * deltaTime;
        }
        else {
            mTimer -= 1/mInterpTime * deltaTime;
        }
    }

    // Set rotation
    SetQuatRotation(Quaternion::Slerp(mEndQ, mStartQ, mTimer));
    
    // Should the rotation be reversed?
    if (mTimer > 1.0f || mTimer < 0.0f) {
        mPause = true;
    }
    
    // Check if CameraMotor is playing
    if (mSoundRotate != -1) {
        AdjustVolume(mSoundRotate);
    }
}

void SecurityCamera::AdjustVolume(int channel)
{
    // Adjust security camera volume
    float dist = (GetGame()->mPlayer->GetPosition() - GetPosition()).Length();
    if (dist > 1500) {
        Mix_Volume(channel, 0);
    }
    else if (dist <= 1500 && dist > 500) {
        float volume = 128.0f - ((dist-500) * 128.0f/1000.0f);
        Mix_Volume(channel, volume);
    }
    else {
        Mix_Volume(channel, 128);
    }
}
