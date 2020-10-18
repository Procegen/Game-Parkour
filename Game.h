#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include "Math.h"

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);
    void AddBlock(class Actor* actor);
    void RemoveBlock(class Actor* actor);
    void AddCamera(class SecurityCamera* camera);
    void RemoveCamera(class SecurityCamera* camera);
    std::vector<class Actor*> GetBlocks() const { return mBlocks; }
    std::vector<class SecurityCamera*> GetCameras() const { return mCameras; }
    
	Mix_Chunk* GetSound(const std::string& fileName);

    int GetTexIndex() { return mTexIndex; }
    void SetTexIndex(int texIndex) { mTexIndex = texIndex; }
	class Renderer* GetRenderer() {	return mRenderer; }
    int GetMusic() const { return mMusic; }
    void SetMusic(int music) { mMusic = music; }
    void SetNextLevel(std::string nextLevel) { mNextLevel = nextLevel; }
    
    class Player* mPlayer;
    class HeightMap* mHeightMap;
    // Checkpoint
    std::queue <class Checkpoint*> mCheckpoints;
    
    void LoadNextLevel();
    
    // HUD variables
    void UpdateCheckpoint();
    class Texture* mCheckpointText;
    float mTime;
    float mCheckpointTimer;
    int mCoinCounter;
    bool mCheckpointOn;
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();
    int mTexIndex = 1;

	std::unordered_map<std::string, Mix_Chunk*> mSounds;
    int mMusic;
	// All the actors in the game
	std::vector<class Actor*> mActors;
    std::vector<class Actor*> mBlocks;
    
	class Renderer* mRenderer;
	Uint32 mTicksCount;
	bool mIsRunning;
    std::string mNextLevel;
    std::vector<class SecurityCamera*> mCameras;
};
