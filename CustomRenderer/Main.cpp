#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include <iostream>
#include <ctime>

#include "Base/Renderer.h"
#include "Base/RenderSettings.h"
#include "Base/Timer.h"
#include "Base/SceneManager.h"

#include "Scenes/TestScene.h"
#include "Scenes/ReflectiveSpheresScene.h"
#include "Scenes/RefractionScene.h"
#include "Scenes/TeapotScene.h"
#include "Scenes/GlassScene.h"
#include "Scenes/GIScene.h"

using namespace std;

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // check memory leaks
	//_CrtSetBreakAlloc(669549);
	//create sdl window and renderer
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	
	std::srand(time(nullptr));
	
	//settings
	const float downScaling = 1.f / 1.f;

	RenderSettings settings;
	settings.LoadFromFile("../Data/RenderSettings.txt");
	settings.DownScaleRender(downScaling);

	Renderer::GetInstance()->Init(settings);
	
	//create SDL window
	SDL_CreateWindowAndRenderer(settings.screenWidth, settings.screenHeight, 0, &window, &renderer);
	SDL_SetWindowTitle(window, "TDR - Dieter Tack");

	SDL_SetRenderDrawColor(renderer, settings.clearColor.r, settings.clearColor.g, settings.clearColor.b, 255);
	SDL_RenderClear(renderer);

	//create rendertarget texture
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, settings.texWidth, settings.texHeight);

	// Check that the window was successfully created
	if (window == nullptr)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	
	Timer::Init();

	//initialize renderer
	Renderer* sceneRenderer = Renderer::GetInstance();

	sceneRenderer->DrawText(renderer, "Loading scene data...", "../Data/arial.ttf", 20,20);
	SDL_RenderPresent(renderer);

	//Set up scenes
	SceneManager::GetInstance()->AddScene(new TestScene());
	SceneManager::GetInstance()->AddScene(new ReflectiveSpheresScene());
	SceneManager::GetInstance()->AddScene(new RefractionScene());
	SceneManager::GetInstance()->AddScene(new GlassScene());
	SceneManager::GetInstance()->AddScene(new GIScene());
	SceneManager::GetInstance()->AddScene(new TeapotScene());

	SceneManager::GetInstance()->SetActiveScene(0);

	const float camSpeed = 4.f;

	bool quitApplication = false;
	float lastPresent = 0.f;

	while (!quitApplication)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			//check for keypresses
			switch (event.type)
			{
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					quitApplication = true;
				}
				//load next scene
				if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
				{
					if (SceneManager::GetInstance()->NextScene())
					{
						sceneRenderer->DrawText(renderer, "Calculating first iteration...", "../Data/arial.ttf", 20, 20);
						SDL_RenderPresent(renderer);
					}
				}
				//load previous scene
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
				{
					if (SceneManager::GetInstance()->PreviousScene())
					{
						sceneRenderer->DrawText(renderer, "Calculating first iteration...", "../Data/arial.ttf", 20, 20);
						SDL_RenderPresent(renderer);
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_D)
				{
					auto activeScene = SceneManager::GetInstance()->GetActiveScene();
					activeScene->MoveCamera(Vec3(camSpeed, 0, 0) * Timer::GetDeltaTime());
					Renderer::GetInstance()->ClearImage();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_A)
				{
					auto activeScene = SceneManager::GetInstance()->GetActiveScene();
					activeScene->MoveCamera(Vec3(-camSpeed, 0, 0) * Timer::GetDeltaTime());
					Renderer::GetInstance()->ClearImage();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_W)
				{
					auto activeScene = SceneManager::GetInstance()->GetActiveScene();
					activeScene->MoveCamera(Vec3(0, 0, camSpeed) * Timer::GetDeltaTime());
					Renderer::GetInstance()->ClearImage();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_S)
				{
					auto activeScene = SceneManager::GetInstance()->GetActiveScene();
					activeScene->MoveCamera(Vec3(0, 0, -camSpeed) * Timer::GetDeltaTime());
					Renderer::GetInstance()->ClearImage();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_E)
				{
					auto activeScene = SceneManager::GetInstance()->GetActiveScene();
					activeScene->MoveCamera(Vec3(0, camSpeed, 0) * Timer::GetDeltaTime());
					Renderer::GetInstance()->ClearImage();
					sceneRenderer->RenderScene();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_Q)
				{
					auto activeScene = SceneManager::GetInstance()->GetActiveScene();
					activeScene->MoveCamera(Vec3(0, -camSpeed, 0) * Timer::GetDeltaTime());
					Renderer::GetInstance()->ClearImage();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_X)
				{
					auto activeScene = SceneManager::GetInstance()->GetActiveScene();
					activeScene->RotateCamera(Vec3(0, camSpeed / 2.f, 0) * Timer::GetDeltaTime());
					Renderer::GetInstance()->ClearImage();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_Z)
				{
					auto activeScene = SceneManager::GetInstance()->GetActiveScene();
					activeScene->RotateCamera(Vec3(0, -camSpeed / 2.f, 0) * Timer::GetDeltaTime());
					Renderer::GetInstance()->ClearImage();
				}

				if (event.key.keysym.scancode == SDL_SCANCODE_UP)
				{
					Renderer::GetInstance()->NextRenderMode();
				}

				if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
				{
					Renderer::GetInstance()->PreviousRenderMode();
				}

				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
				{
					Renderer::GetInstance()->ClearImage();
				}
				break;
			/*case SDL_KEYUP:
				printf("Key release detected\n");
				break;*/
			case SDL_QUIT:
				quitApplication = true;
				break;
			}
		}

		//render scene to pixelbuffer
		sceneRenderer->RenderScene();

		//present texture on screen
		if (Timer::GetTotalTime() - lastPresent > 1 / 30.f)
		{
			SDL_UpdateTexture(texture, nullptr, sceneRenderer->GetPixels(), settings.texWidth * 4);
			SDL_RenderCopy(renderer, texture, nullptr, nullptr);
			SDL_RenderPresent(renderer);
			lastPresent = Timer::GetTotalTime();
		}

		Timer::EndFrame();
	}

	Renderer::Destroy();
	SceneManager::Destroy();
	MaterialManager::DestroyRemainingTextures();

	//Clean up
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);

	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	return 0;
}