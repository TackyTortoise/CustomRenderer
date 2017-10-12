#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "SDL.h"
#include "SDL_ttf.h"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

#include "Math.h"
#include "Objects/Object.h"
#include "Objects/Sphere.h"
#include "Objects/Plane.h"
#include "Base/Scene.h"
#include "Base/Renderer.h"
#include "Base/RenderSettings.h"
#include "Base/Timer.h"
#include "Math/Matrix4x4.h"

using namespace std;

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // check memory leaks
	//_CrtSetBreakAlloc(196);
	//create sdl window and renderer
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	std::srand(time(nullptr));
	
	//settings
	const float downScaling = 2.f / 1.f;

	RenderSettings settings;
	settings.screenWidth = 800;
	settings.screenHeight = 600;
	settings.texWidth = settings.screenWidth / downScaling;
	settings.texHeight = settings.screenHeight / downScaling;
	settings.blockCount = 75;
	settings.shadowSampleCount = 1;
	settings.cameraFOV = 60;
	settings.maxRenderDepth = 10;
	
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

	bool quitApplication = false;

	//creation of scene
	int sceneNumber = 0;
	int totalScenes = 4;
	Scene* testScene = new Scene(sceneNumber);
	testScene->SetupCamera(settings);
	//cout << cam->GetTransform().GetRotationMatrix() << std::endl;
	//cam->GetTransform().SetRotation(Vec3(M_PI / 5, 0, 0));

	Renderer* sceneRenderer = new Renderer();
	sceneRenderer->Init(settings);

	sceneRenderer->SetActiveScene(testScene);

	Timer::Init();

	while (!quitApplication)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			//handle your event here 
			switch (event.type)
			{
			case SDL_KEYDOWN:
				std::cout << "Key press detected\n";
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					quitApplication = true;
				}
				//load next scene
				if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
				{
					++sceneNumber;
					sceneNumber %= totalScenes;
					delete testScene;
					testScene = new Scene(sceneNumber);
					testScene->SetupCamera(settings);
					sceneRenderer->SetActiveScene(testScene);
				}
				//load previous scene
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
				{
					--sceneNumber;
					if (sceneNumber < 0)
						sceneNumber = totalScenes - 1;
					delete testScene;
					testScene = new Scene(sceneNumber);
					testScene->SetupCamera(settings);
					sceneRenderer->SetActiveScene(testScene);
				}
				break;

			case SDL_KEYUP:
				printf("Key release detected\n");
				break;
			case SDL_QUIT:
				quitApplication = true;
				break;
			}
		}

		//render scene to texture
		sceneRenderer->RenderScene();

		//present texture on screen
		SDL_UpdateTexture(texture, nullptr, &sceneRenderer->GetPixels()[0], settings.texWidth * 4);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		Timer::EndFrame();
		//std::cout << Timer::GetTotalTime() << std::endl;
	}

	delete testScene;
	delete sceneRenderer;

	// Close and destroy the window
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);

	SDL_Quit();
	return 0;
}