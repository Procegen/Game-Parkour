#include "HUD.h"
#include "UIComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Actor.h"
#include <SDL/SDL_ttf.h>
#include "Font.h"
#include "Texture.h"
#include "Checkpoint.h"
#include "SecurityCamera.h"
#include <string>
#include <iostream>

HUD::HUD(Actor* owner)
:UIComponent(owner)
{
    mFont = new Font();
    mFont->Load("Assets/Inconsolata-Regular.ttf");
    mTimerText = mFont->RenderText("00:00.00");
    
    std::string text = std::to_string(mOwner->GetGame()->mCoinCounter) + "/55";
    if (mOwner->GetGame()->mCoinCounter < 10) {
        text = "0" + std::to_string(mOwner->GetGame()->mCoinCounter) + "/55";
    }
    mCoinText = mFont->RenderText(text);
    mRadarBG = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Radar.png");
    mRadarArrow = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/RadarArrow.png");
    mBlip = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Blip.png");
}

HUD::~HUD()
{
    mFont->Unload();
    delete mFont;
}

void HUD::Update(float deltaTime)
{
    mOwner->GetGame()->mTime += deltaTime;
    mTimerText->Unload();
    delete mTimerText;
    
    // Format timer time into string
    std::string m = std::to_string((int)mOwner->GetGame()->mTime / 60);
    std::string s = std::to_string((int)mOwner->GetGame()->mTime % 60);
    std::string f = std::to_string((mOwner->GetGame()->mTime-(int)mOwner->GetGame()->mTime) * 100);
    f = f.substr(0,2);
    if (m.length() == 1) {
        m = "0" + m;
    }
    if (s.length() == 1) {
        s = "0" + s;
    }
    if (f.substr(1,2) == ".") {
        f = "0" + f.substr(0,1);
    }
    std::string text = m + ":" + s + "." + f;
    mTimerText = mFont->RenderText(text);
        
    // Checkpoint update
    if (mOwner->GetGame()->mCheckpointOn) {
        mOwner->GetGame()->mCheckpointTimer += deltaTime;
        // Upload text and delete
        if (mOwner->GetGame()->mCheckpointTimer >= 5.0f) {
            mOwner->GetGame()->mCheckpointText->Unload();
            delete mOwner->GetGame()->mCheckpointText;
            mOwner->GetGame()->mCheckpointTimer = 0.0f;
            mOwner->GetGame()->mCheckpointOn = false;
        }
    }
}

void HUD::Draw(Shader* shader)
{
    DrawTexture(shader, mTimerText, Vector2(-420.0f, -325.0f));
    DrawTexture(shader, mCoinText, Vector2(-420.0f, -290.0f));
    if (mOwner->GetGame()->mCheckpointOn && mOwner->GetGame()->mCheckpointText != nullptr) {
        DrawTexture(shader, mOwner->GetGame()->mCheckpointText, Vector2::Zero);
    }
    DrawTexture(shader, mRadarBG, Vector2(400.0f, -280.0f));
    DrawTexture(shader, mRadarArrow, Vector2(400.0f, -280.0f));
    UpdateRadar(shader);
}

void HUD::SetCheckpointText(std::string text)
{
    mOwner->GetGame()->mCheckpointText = mFont->RenderText(text);
}

void HUD::UpdateCoin()
{
    mOwner->GetGame()->mCoinCounter++;
    mCoinText->Unload();
    delete mCoinText;
    
    std::string text;
    if (mOwner->GetGame()->mCoinCounter < 10) {
        text = "0" + std::to_string(mOwner->GetGame()->mCoinCounter) + "/55";
    }
    else {
        text = std::to_string(mOwner->GetGame()->mCoinCounter) + "/55";
    }
    mCoinText = mFont->RenderText(text);
}

void HUD::UpdateRadar(Shader* shader)
{
    float range = 1500.0f;
    float radius = 92.0f;
    
    // Player calculation
    Vector2 newPlayerFwd = Vector2(mOwner->GetForward().x, mOwner->GetForward().y);
    float angle = Math::Atan2(newPlayerFwd.y, newPlayerFwd.x);
    Matrix3 rotMatrix = Matrix3::CreateRotation(angle);
    
    for (auto i : mOwner->GetGame()->GetCameras()) {
        // Blip position calculation
        Vector2 player2D = Vector2(mOwner->GetPosition().y, mOwner->GetPosition().x);
        Vector2 camera2D = Vector2(i->GetPosition().y, i->GetPosition().x);
        Vector2 playerToCam = camera2D - player2D;
        float distance = playerToCam.Length();
        
        // Blip rotation calculation
        Vector2 newBlipFwd = Vector2(i->GetQuatForward().y, i->GetQuatForward().x);
        Vector2 blipRot = Vector2::Transform(newBlipFwd, rotMatrix);
        float blipAngle = Math::Atan2(blipRot.y, blipRot.x);
        
        // Camera in range
        if (distance <= range) {
            playerToCam = playerToCam * (1/range);
            playerToCam = playerToCam * radius;
            Vector2 radarCamPos = Vector2::Transform(playerToCam, rotMatrix);
            radarCamPos += Vector2(400.0f, -280.0f);
            DrawTexture(shader, mBlip, radarCamPos, 1.0f, blipAngle);
        }
    }
}
