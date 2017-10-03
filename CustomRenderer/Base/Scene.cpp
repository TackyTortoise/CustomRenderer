#include "Scene.h"
#include "../Objects/Sphere.h"
#include "../Objects/Plane.h"
#include "../Objects/AABox.h"

Scene::Scene()
{
	/*Plane* pTest = new Plane();
	pTest->SetPosition({ 0, -5, 0 });
	pTest->SetReflective(.1f);
	m_Objects.push_back(pTest);
	pTest = new Plane();
	pTest->SetPosition({ 0,50,0 });
	pTest->SetBaseColor(Color(255, 0, 0));
	m_Objects.push_back(pTest);

	Sphere* s = new Sphere(Vec3(0, 0, 10), 2.5, Color(255));
	s->SetTransparent(.5f);
	m_Objects.push_back(s);
	s = new Sphere(Vec3(5, 0, 20), 2.5, Color(255));
	s->SetReflective(.5f);
	m_Objects.push_back(s);

	m_Lights.push_back({ 10,10,0 });*/
	LoadTestScene();
}


Scene::~Scene()
{
	for(int i = 0; i < m_Objects.size(); ++i)
	{
		delete m_Objects[i];
		m_Objects[i] = nullptr;
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

void Scene::LoadTestScene()
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
	m_Objects[4]->SetReflective(1.f);
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

	m_Lights.push_back({ 10,30,0 });
}
