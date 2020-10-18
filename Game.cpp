//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include <iostream>
#include "Renderer.h"
#include "Random.h"
#include "Player.h"
#include "MeshComponent.h"
#include "CameraComponent.h"
#include "LevelLoader.h"
#include "Arrow.h"
#include "HUD.h"
#include "Checkpoint.h"
#include <SDL/SDL_ttf.h>

Game::Game()
:mIsRunning(true)
{
    mCheckpointTimer = 0;
    mCoinCounter = 0;
    mTime = 0;
    mCheckpointOn = false;
}

bool Game::Initialize()
{
    float WIDTH = 1024.0f;
    float HEIGHT = 768.0f;
    
	Random::Init();
    TTF_Init();
    
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
    
    // Dynamically allocate a Renderer (pass in this)
    mRenderer = new Renderer(this);
    // Initialize Renderer
    if (!mRenderer->Initialize(WIDTH, HEIGHT)) {
        return false;
    }

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(32);
    Mix_GroupChannels(22, 31, 1);

	LoadData();

	mTicksCount = SDL_GetTicks();
    
    SDL_SetRelativeMouseMode(SDL_TRUE); // Enable relative mouse mode
    SDL_GetRelativeMouseState(nullptr, nullptr); // Clear any saved values
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
        if (!mNextLevel.empty()) {
            LoadNextLevel();
        }
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.01667f)
	{
		deltaTime = 0.01667f;
	}
	mTicksCount = SDL_GetTicks();
	
	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any actors to destroy to a temp vector
	std::vector<Actor*> destroyActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			destroyActors.emplace_back(actor);
		}
	}

	// Delete the destroyed actors (which will
	// remove them from mActors)
	for (auto actor : destroyActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	// tell renderer to draw
    mRenderer->Draw();
    
}

void Game::LoadData()
{
    // Variables
    float fovY = 1.22f, width = 1024.0f, height = 768.0f, near = 10.0f, far = 10000.0f;
    Vector3 eye = Vector3(-300, 0, 100);
    Vector3 target = Vector3(20, 0, 0);
    Vector3 up = Vector3::UnitZ;
    
    // Projection Matrix
    Matrix4 projectMatrix = Matrix4::CreatePerspectiveFOV(fovY, width, height, near, far);
    mRenderer->SetProjectionMatrix(projectMatrix);
    
    // View Matrix
    Matrix4 viewMatrix = Matrix4::CreateLookAt(eye, target, up);
    mRenderer->SetViewMatrix(viewMatrix);
    
    // Load level
    LevelLoader::Load(this, "Assets/Tutorial.json");
    
    // Player music
    Mix_Chunk *sample = GetSound("Assets/Sounds/Music.ogg");
    mMusic = Mix_PlayChannel(-1, sample, -1);
    
    new Arrow(this, mPlayer->mParent);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy sounds
	for (auto s : mSounds)
	{
		Mix_FreeChunk(s.second);
	}
	mSounds.clear();
}

Mix_Chunk* Game::GetSound(const std::string& fileName)
{
	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

void Game::Shutdown()
{
	UnloadData();
	Mix_CloseAudio();
    
	// Delete renderer
    mRenderer->Shutdown();
    delete mRenderer;
    
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mActors.end() - 1;
		std::iter_swap(iter, iter2);
		mActors.pop_back();
	}
}

void Game::AddBlock(Actor* actor)
{
    mBlocks.emplace_back(actor);
}

void Game::RemoveBlock(Actor* actor)
{
    auto iter = std::find(mBlocks.begin(), mBlocks.end(), actor);
    if (iter != mBlocks.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        auto iter2 = mBlocks.end() - 1;
        std::iter_swap(iter, iter2);
        mBlocks.pop_back();
    }
}

void Game::LoadNextLevel()
{
    // Delete all actors in the world
    while (!mActors.empty()) {
        delete mActors.back();
    }
    // Clear out the checkpoint queue
    while (!mCheckpoints.empty()) {
        mCheckpoints.pop();
    }
    std::cout << mNextLevel << std::endl;
    LevelLoader::Load(this, mNextLevel);
    new Arrow(this, mPlayer->mParent);
    mNextLevel.clear();
}


void Game::UpdateCheckpoint()
{
    mCheckpointOn = true;
    mCheckpointTimer = 0.0f;
    if (!mCheckpoints.empty()) {
        mPlayer->GetComponent<HUD>()->SetCheckpointText(mCheckpoints.front()->GetText());
    }
}

void Game::AddCamera(class SecurityCamera *camera)
{
    mCameras.emplace_back(camera);
}

void Game::RemoveCamera(class SecurityCamera *camera)
{
    auto iter = std::find(mCameras.begin(), mCameras.end(), camera);
    if (iter != mCameras.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        auto iter2 = mCameras.end() - 1;
        std::iter_swap(iter, iter2);
        mCameras.pop_back();
    }
}
