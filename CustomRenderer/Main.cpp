#include "SDL.h"
#include "SDL_ttf.h"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

#include "Object.h"
#include "Sphere.h"
#include "Math.h"
#include "Plane.h"
using namespace std;

//#define TEST

int main(int argc, char* argv[])
{
#ifndef TEST
	SDL_Window *window;                    // Declare a pointer
	SDL_Renderer *renderer;
	SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
	TTF_Init();

	srand(time(nullptr));

	const unsigned int screenWidth = 800;
	const unsigned int screenHeight = 600;
	const float aspectRatio = screenWidth / static_cast<float>(screenHeight);
	const float fov = 60;
	const float shadowIntensity = .8f;

	SDL_CreateWindowAndRenderer(screenWidth, screenHeight, 0, &window, &renderer);
	SDL_SetWindowTitle(window, "TDR - Dieter Tack");

	Vec3 lightSource(10, 50, 0);

	//text setup
	TTF_Font* font = TTF_OpenFont("arial.ttf", 25);
	SDL_Color color = { 255, 255, 255 };
	SDL_Surface * textSurface = TTF_RenderText_Solid(font, "FPS: 00 Iterations: 00000 Total Time: 000000", color);
	SDL_Texture * textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(textTexture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { 0, 0, texW, texH };

	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_RenderClear(renderer);
	//SDL_RenderPresent(renderer);

	const float downScaling = 1.f;
	const unsigned int texWidth = static_cast<int>(screenWidth / downScaling);
	const unsigned int texHeight = static_cast<int>(screenHeight / downScaling);

	const float blockCount = 75;
	const float regionSizeX = texWidth / blockCount;
	const float regionSizeY = texHeight / blockCount;
	SDL_Texture* texture = SDL_CreateTexture
	(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		texWidth, texHeight
	);

	Color* pixels = new Color[texWidth * texHeight];

	// Check that the window was successfully created
	if (window == nullptr) {
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}


	bool quitApplication = false;
	Uint64 lastFrameEndTimeCount = SDL_GetPerformanceCounter();

	vector<Object*> vObjects;
	vObjects.push_back(new Sphere(Vec3(0, 0, -50), 2, Color(255, 50, 170))); // small pink in back
	vObjects.push_back(new Sphere(Vec3(5, 1, -15), 2, Color(70, 80, 255))); // reflective blue
	vObjects.push_back(new Sphere(Vec3(-5, 0, -10), 2, Color(60, 255, 100))); // transparant green
	vObjects.push_back(new Sphere(Vec3(-4, 3.9, -8.5), .5, Color(255, 255, 70))); // yellow in air
	vObjects.push_back(new Sphere(Vec3(-3.2, -.25f, -10), .75, Color(140, 16, 140))); // purple in green
	vObjects.push_back(new Sphere(Vec3(5, -2.5, -13), 1, Color(180, 120, 255))); // small purple
	vObjects.push_back(new Sphere(Vec3(-2.5, -1, -45), 2, Color(124, 20, 77))); // burgundi
	vObjects.push_back(new Sphere(Vec3(-3, -2.5, -10), .8f, Color(108, 92, 50))); // kaky
	vObjects.push_back(new Sphere(Vec3(4, 2, -9.5), .5, Color(244, 101, 44))); // orange

	int tx = -20;
	for (int i = 0; i < 10; ++i)
	{
		vObjects.push_back(new Sphere(Vec3(tx, 3 + rand()%3, -55 + tx), 2, Color(rand() % 256, rand() % 256, rand() % 256)));
		vObjects.back()->SetReflective(static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
		tx += 4;
	}

	vObjects[2]->SetTransparent(.5f);
	vObjects[6]->SetReflective(.2f);
	vObjects[6]->SetTransparent(.2f);
	vObjects[1]->SetReflective(.7f);
	vObjects[4]->SetReflective(1.f);
	Plane* pTest = new Plane();
	pTest->SetPosition({ 0, -5, 0 });
	pTest->SetReflective(.1f);
	vObjects.push_back(pTest);

	Object** objects = &vObjects[0];
	//pTest.SetNormal(Vec3(0, 1, .1).Normalize());

	unsigned iterations = 0;
	float totalTime = 0.f;

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
			default:
				break;
			}
		}

		++iterations;

		float camAngleTan = tan(fov / 2 * M_PI / 180.f);
		for (int x = 0; x < texWidth - regionSizeX + 1; x += regionSizeX)
		{
			for (int y = 0; y < texHeight - regionSizeY + 1; y += regionSizeY)
			{
				//choose random pixel within block
				float rX;
				do
				{
					rX = x + rand() % (int)regionSizeX;
				} while (rX >= texWidth);

				float rY;
				do
				{
					rY = y + rand() % (int)regionSizeY;
				} while (rY >= texHeight);

				//transform pixel to camera space
				float cX = (2 * ((rX + .5f) / texWidth) - 1) * camAngleTan * aspectRatio;
				float cY = (1 - 2 * ((rY + .5f) / texHeight)) * camAngleTan;

				//create ray from camera to pixel
				Vec3 rayOrg(0); //start from camera position 0,0,0
				Vec3 rayDir = Vec3(cX, cY, -1.f) - rayOrg;
				//multiply with camtoworld if cam is not in 0 transform
				rayDir.Normalize();

				//check if hit object
				float d = 0;
				bool hit;
				float shortD = std::numeric_limits<float>::max();
				Object* closeObject = nullptr;
				for (int i = 0; i < vObjects.size(); ++i)
				{
					hit = objects[i]->isHit(rayOrg, rayDir, d);
					if (hit && d < shortD)
					{
						shortD = d;
						closeObject = objects[i];
					}
				}

				//check shadow from hitpoint
				bool shadow = false;
				Vec3 hitPoint = rayDir * shortD;
				Vec3 normalOnHit = Vec3(0);
				
				if (closeObject != nullptr)
					normalOnHit = closeObject->GetNormalOnHit(hitPoint);

				shortD = std::numeric_limits<float>::max();
				auto toLight = (lightSource - hitPoint).Normalize();
				for (int i = 0; i < vObjects.size(); ++i)
				{
					hit = objects[i]->isHit(hitPoint, toLight, d);
					if (hit && d < shortD)
					{
						shortD = d;
						if (objects[i] != closeObject)
						{
							shadow = true;
						}
					}
				}


				int ind = rX + texWidth * rY;
				//calculate color
				if (closeObject != nullptr)
				{
					Color col = closeObject->GetBaseColor();
					auto lightRay = lightSource - closeObject->GetPosition();
					lightRay.Normalize();
					auto halfVec = lightRay + -rayDir;
					auto normal = normalOnHit;
					float intensity = normal.Dot(halfVec);
					intensity = Math::Clamp(intensity, 0.15f, 1.f);
					//auto b = normal.Dot(lightRay) > 0 ? 1 : 0;
					//auto spec = b * pow(Math::Clamp(normal.Dot(halfVec), 0,1), 2);

					//check transparancy
					auto trans = closeObject->GetTransparancy();
					shortD = numeric_limits<float>::max();
					if (trans > 0)
					{
						col *= trans;
						Object* closeTrans = nullptr;
						for (int i = 0; i < vObjects.size(); ++i)
						{
							if (objects[i] == closeObject)
								continue;
							hit = objects[i]->isHit(hitPoint, rayDir, d);
							if (hit && d < shortD)
							{
								shortD = d;
								closeTrans = objects[i];
							}
						}
						if (closeTrans != nullptr)
						{
							Color otherColor = closeTrans->GetBaseColor();
							col += otherColor * trans;
						}
					}

					//check reflection
					auto refl = closeObject->GetReflective();
					shortD = numeric_limits<float>::max();
					if (refl > 0)
					{
						Object* closeRef = nullptr;
						for (int i = 0; i < vObjects.size(); ++i)
						{
							if (objects[i] == closeObject)
								continue;

							auto objectNormal = normalOnHit;
							auto viewToHit = -(hitPoint - Vec3(0)).Normalize();
							auto reflectedRay = viewToHit.ReflectAround(objectNormal);

							hit = objects[i]->isHit(hitPoint, reflectedRay, d);
							if (hit && d < shortD)
							{
								shortD = d;
								closeRef = objects[i];
							}
						}

						if (closeRef != nullptr)
						{
							auto otherCol = closeRef->GetBaseColor();
							col = col * (1 - refl) + otherCol * refl;
						}
					}
					
					//auto sc = Color(255,255,255) * spec;
					//auto dc = col * intensity;
					//col = sc.ClampAdd(dc);

					//make shadow if necessary
					if (shadow)
					{
						col *= 1.f - shadowIntensity;
						//col *= intensity;
					}
					pixels[ind] = col * intensity;
				}
				//clear the pixel if no hit
				else
				{
					pixels[ind] = Color(0, 0, 0);
				}
			}
		}

		SDL_UpdateTexture
		(
			texture,
			NULL,
			&pixels[0],
			texWidth * 4
		);

		Uint64 now = SDL_GetPerformanceCounter();
		auto diff = (float)(now - lastFrameEndTimeCount) / SDL_GetPerformanceFrequency();
		totalTime += diff;
		lastFrameEndTimeCount = now;
		int fps = 1 / diff;

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		string text = "FPS " + to_string(fps) + " Iterations: " + to_string(iterations) + " Total Time: " + to_string(totalTime);
		textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
		textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_RenderCopy(renderer, textTexture, NULL, &dstrect);
		SDL_RenderPresent(renderer);
		//SDL_Delay(500);
	}

	// Close and destroy the window
	SDL_DestroyWindow(window);

	delete[] pixels;
	for (int i = 0; i < vObjects.size(); ++i)
	{
		delete vObjects[i];
		vObjects[i] = nullptr;
	}
	objects = nullptr;

	// Clean up
	SDL_DestroyTexture(textTexture);
	SDL_FreeSurface(textSurface);

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	return 0;
#endif

#ifdef TEST
	Vec3 v(1, 2, 3);
	Vec3 n(3, 2, 1);
	auto t = v.ReflectAround(n);
	return 0;
#endif
}