#include "Scene.h"
#include "../Objects/Sphere.h"
#include "../Objects/Plane.h"
#include "../Objects/AABox.h"
#include <SDL_stdinc.h>
#include "../Objects/Triangle.h"

Scene::Scene()
{
	LoadTestScene(1);
}


Scene::~Scene()
{
	for(int i = 0; i < m_Objects.size(); ++i)
	{
		delete m_Objects[i];
		m_Objects[i] = nullptr;
	}

	for (int i = 0; i <m_Lights.size(); ++i)
	{
		delete m_Lights[i];
		m_Lights[i] = nullptr;
	}

	delete m_Camera;
	m_Camera = nullptr;
}

void Scene::SetupCamera(const float fov, const float width, const float height)
{
	if (m_Camera != nullptr)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	m_Camera = new Camera(fov, width / height);
}

void Scene::LoadTestScene(int m)
{
	if (m == 1)
	{
		m_Objects.push_back(new Sphere(Vec3(0, 0, 50), 2, Color(255, 50, 170))); // small pink in back
		m_Objects.push_back(new Sphere(Vec3(5, 0, 15), 2, Color(70, 80, 255))); // reflective blue
		m_Objects.push_back(new Sphere(Vec3(-5, 0, 10), 2, Color(60, 255, 100))); // transparant green
		m_Objects.push_back(new Sphere(Vec3(-4, 2.9, 8.5), .5, Color(255, 255, 70))); // yellow in air
		m_Objects.push_back(new Sphere(Vec3(-3.2, -.25f, 10), .75, Color(140, 16, 140))); // purple in green
		m_Objects.push_back(new Sphere(Vec3(5, -2.5, 13), 1, Color(180, 120, 255))); // small purple
		m_Objects.push_back(new Sphere(Vec3(-2.5, -1, 45), 2, Color(124, 20, 77))); // burgundi
		m_Objects.push_back(new Sphere(Vec3(-3, -2.5, 10), .8f, Color(108, 92, 50))); // kaky
		m_Objects.push_back(new Sphere(Vec3(4, 2, 9.5), .5, Color(244, 101, 44))); // orange

		int tx = -20;
		for (int i = 0; i < 10; ++i)
		{
			m_Objects.push_back(new Sphere(Vec3(tx, 3 + rand() % 3, 55 - tx), 2, Color(rand() % 256, rand() % 256, rand() % 256)));
			m_Objects.back()->SetReflective(static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
			tx += 4;
		}

		m_Objects[2]->SetTransparent(.5f);
		m_Objects[6]->SetReflective(.2f);
		m_Objects[6]->SetTransparent(.2f);
		m_Objects[1]->SetReflective(.99f);
		m_Objects[4]->SetReflective(.5f);
		m_Objects[4]->SetMetallic(true);
		Plane* pTest = new Plane();
		pTest->SetPosition({ 0, -5, 0 });
		pTest->SetReflective(.1f);
		m_Objects.push_back(pTest);

		pTest = new Plane();
		pTest->SetPosition({ 0,50,0 });
		pTest->SetBaseColor(Color(255, 0, 0));
		m_Objects.push_back(pTest);

		m_Objects.push_back(new AABox(Vec3(-5, -.5, 20), 7, 7, 8));
		m_Objects.back()->SetReflective(.85f);
		m_Objects.back()->SetBaseColor(Color(255, 215, 0));
		//m_Objects.push_back(new AABox(Vec3(-1, -3.5, 20), 5, 2, 5));
		m_Objects.push_back(new AABox(Vec3(5, 0, 20), 5, 6.5, 5, Color(29, 108, 129)));


		//for (auto o : m_Objects)
		//{
		//	o->SetReflective(1.f);
		//}

		m_Lights.push_back(new Light({10,30,0}, {0,-1,0}, 4,4));
	}
	else if (m == 2)
	{
		Plane* pTest = new Plane();
		pTest->SetPosition({ 0, -5, 0 });
		pTest->SetReflective(.3f);
		m_Objects.push_back(pTest);
		pTest = new Plane();
		pTest->SetPosition({ 0,50,0 });
		pTest->SetBaseColor(Color(0, 0, 128));
		m_Objects.push_back(pTest);

		Sphere* s = nullptr;
		for (int i = 0; i <= 15; ++i)
		{
			s = new Sphere(Vec3(cos(2 * M_PI / 15 * i) * 9, -4, sin(2 * M_PI / 15 * i) * 9 + 20), 1.5f, Color(rand() % 255, rand() % 255, rand() % 255));
			s->SetReflective(.8f);
			s->SetMetallic(true);
			//s->SetRefractive(1.5f);
			//s->SetTransparent(1.f);
			m_Objects.push_back(s);

			s = new Sphere(Vec3(cos(2 * M_PI / 15 * i) * 9, 4, sin(2 * M_PI / 15 * i) * 9 + 20), 1.5f, Color(rand() % 255, rand() % 255, rand() % 255));
			s->SetReflective(.8f);
			s->SetMetallic(true);
			//s->SetRefractive(1.5f);
			//s->SetTransparent(1.f);
			m_Objects.push_back(s);
		}
		s = new Sphere(Vec3(0, 0, 20), 3.f, Color(rand() % 255, rand() % 255, rand() % 255));
		s->SetReflective(.8f);
		s->SetMetallic(true);
		//s->SetRefractive(1.5f);
		//s->SetTransparent(1.f);
		m_Objects.push_back(s);

		m_Lights.push_back(new Light({ 7,0,20 }, { 0,-1,0 }, 2, 1));
	}
	else if (m == 3)
	{
		Plane* pTest = new Plane();
		pTest->SetPosition({ 0, -20, 0 });
		pTest->SetBaseColor(Color(200, 200, 0));
		//pTest->SetReflective(.3f);
		m_Objects.push_back(pTest);
		pTest = new Plane();
		pTest->SetPosition({ 0,20,0 });
		pTest->SetBaseColor(Color(0, 200, 0));
		m_Objects.push_back(pTest);		

		pTest = new Plane;
		pTest->SetPosition({ 20,0,0 });
		pTest->SetNormal({ -1,0,0 });
		pTest->SetBaseColor(Color(0, 0, 200));
		m_Objects.push_back(pTest);

		pTest = new Plane;
		pTest->SetPosition({ -20,0,0 });
		pTest->SetNormal({ 1,0,0 });
		pTest->SetBaseColor(Color(200, 0, 0));
		m_Objects.push_back(pTest);

		pTest = new Plane;
		pTest->SetPosition({ 0,0,100 });
		pTest->SetNormal({ 0,0,-1 });
		pTest->SetBaseColor(Color(20));
		m_Objects.push_back(pTest);

		float rv = 1.52f;

		//refractive ball center
		Sphere* s = new Sphere(Vec3(0, 0, 25), 3.f, Color(255));
		s->SetTransparent(1.0f);
		s->SetRefractive(rv);
		m_Objects.push_back(s);
		//refractive top 
		s = new Sphere(Vec3(6, 4, 30), 3.f, Color(255));
		s->SetTransparent(1.0f);
		s->SetRefractive(rv);
		m_Objects.push_back(s);

		s = new Sphere(Vec3(-6, 4, 30), 3.f, Color(255));
		s->SetTransparent(1.0f);
		s->SetRefractive(rv);
		m_Objects.push_back(s);

		s = new Sphere(Vec3(6, -4, 30), 3.f, Color(255));
		s->SetTransparent(1.0f);
		s->SetRefractive(rv);
		m_Objects.push_back(s);

		s = new Sphere(Vec3(-6, -4, 30), 3.f, Color(255));
		s->SetTransparent(1.0f);
		s->SetRefractive(rv);
		m_Objects.push_back(s);
		
		//refractive bottom
		s = new Sphere(Vec3(6, 4, 20), 3.f, Color(255));
		s->SetTransparent(1.0f);
		s->SetRefractive(rv);
		m_Objects.push_back(s);

		s = new Sphere(Vec3(-6, 4, 20), 3.f, Color(255));
		s->SetTransparent(1.0f);
		s->SetRefractive(rv);
		m_Objects.push_back(s);

		s = new Sphere(Vec3(6, -4, 20), 3.f, Color(255));
		s->SetTransparent(1.0f);
		s->SetRefractive(rv);
		m_Objects.push_back(s);

		s = new Sphere(Vec3(-6, -4, 20), 3.f, Color(255));
		s->SetTransparent(1.0f);
		s->SetRefractive(rv);
		m_Objects.push_back(s);
		
		m_Lights.push_back(new Light({ 0,0,40 }, { 0,-1,0 }, 2, 1));
	}
	else if (m == 4)
	{
		Triangle* t = new Triangle(Vec3(-5, -3, 20), Vec3(0, 3, 30), Vec3(5, -3, 20));
		m_Objects.push_back(t);

		m_Lights.push_back(new Light({ 15,15,0 }, { 0,-1,0 }, 2, 1));
	}
}
