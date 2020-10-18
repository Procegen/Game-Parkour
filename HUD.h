#pragma once
#include "Math.h"
#include "UIComponent.h"
#include <string>
#include <SDL/SDL_ttf.h>

class HUD : public UIComponent
{
public:
    HUD(class Actor* owner);
    ~HUD();
    
    void Draw(class Shader* shader) override;
    void Update(float deltaTime) override;
    void UpdateCoin();
    void SetCheckpointText(std::string text);
    void UpdateRadar(class Shader* shader);
private:
    class Font* mFont;
    Texture* mTimerText;
    Texture* mCoinText;
    Texture* mRadarBG;
    Texture* mRadarArrow;
    Texture* mBlip;
    Vector2 mRadarCamPos;
};
