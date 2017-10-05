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
using namespace std;

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // check memory leaks

	//create sdl window and renderer
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	std::srand(time(nullptr));

	const unsigned int screenWidth = 800;
	const unsigned int screenHeight = 600;

	SDL_CreateWindowAndRenderer(screenWidth, screenHeight, 0, &window, &renderer);
	SDL_SetWindowTitle(window, "TDR - Dieter Tack");

	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_RenderClear(renderer);

	//supersampling settings
	const float downScaling = 1 / 1;
	const int blockCount = 75;
	const unsigned int texWidth = static_cast<int>(screenWidth / downScaling);
	const unsigned int texHeight = static_cast<int>(screenHeight / downScaling);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, texWidth, texHeight);

	// Check that the window was successfully created
	if (window == nullptr) {
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	bool quitApplication = false;

	//creation of scene
	const float fov = 60;
	Scene* testScene = new Scene();
	testScene->SetupCamera(fov, screenWidth, screenHeight);
	Renderer* sceneRenderer = new Renderer();
	sceneRenderer->Init(texWidth, texHeight, blockCount);
	sceneRenderer->SetActiveScene(testScene);
	
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
		SDL_UpdateTexture(texture, nullptr, &sceneRenderer->GetPixels()[0], texWidth * 4);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	delete testScene;
	delete sceneRenderer;

	// Close and destroy the window
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);

	SDL_Quit();
	return 0;
}