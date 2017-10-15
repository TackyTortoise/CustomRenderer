#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

#include "Math.h"
#include "Objects/Object.h"
#include "Objects/Sphere.h"
#include "Base/Scene.h"
#include "Base/Renderer.h"
#include "Base/RenderSettings.h"
#include "Base/Timer.h"
#include "Base/SceneManager.h"
#include "Scenes/TestScene.h"
#include "Scenes/ReflectiveSpheresScene.h"
#include "Scenes/RefractionScene.h"
#include "Scenes/TeapotScene.h"

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
	IMG_Init(IMG_INIT_JPG);

	std::srand(time(nullptr));
	
	//settings
	const float downScaling = 1.f / 1.f;

	RenderSettings settings;
	settings.screenWidth = 800;
	settings.screenHeight = 600;
	settings.texWidth = settings.screenWidth / downScaling;
	settings.texHeight = settings.screenHeight / downScaling;
	settings.blockCount = 75;
	settings.shadowSampleCount = 4;
	settings.cameraFOV = 60;
	settings.maxRenderDepth = 10;
	settings.enableSrgb = false;
	
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
	
	//Set up scenes
	SceneManager::GetInstance()->AddScene(new TestScene());
	SceneManager::GetInstance()->AddScene(new ReflectiveSpheresScene());
	SceneManager::GetInstance()->AddScene(new RefractionScene());
	SceneManager::GetInstance()->AddScene(new TeapotScene());

	//initialize renderer
	Renderer* sceneRenderer = Renderer::GetInstance();
	Renderer::GetInstance()->Init(settings);

	SceneManager::GetInstance()->SetActiveScene(0);

	Timer::Init();

	bool quitApplication = false;
	while (!quitApplication)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			//check for keypresses
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
					SceneManager::GetInstance()->NextScene();
				}
				//load previous scene
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
				{
					SceneManager::GetInstance()->PreviousScene();
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
		SDL_UpdateTexture(texture, nullptr, &sceneRenderer->GetPixels()[0], settings.texWidth * 4);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		Timer::EndFrame();
	}

	Renderer::Destroy();
	SceneManager::Destroy();

	//Clean up
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);

	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	return 0;
}