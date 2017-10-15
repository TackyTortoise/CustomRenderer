#include "Scene.h"
#include "../Objects/Sphere.h"
#include "../Objects/Plane.h"
#include "../Objects/AABox.h"
#include <SDL_stdinc.h>
#include "../Objects/Triangle.h"
#include "../Objects/AssimpModel.h"

Scene::Scene(const int n)
{
	LoadTestScene(n);
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

Camera* Scene::SetupCamera(const RenderSettings& rs)
{
	if (m_Camera != nullptr)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	m_Camera = new Camera(rs.cameraFOV, rs.screenWidth / static_cast<float>(rs.screenHeight));
	//m_Camera->GetTransform()->SetTranslation({0,0,0 });
	//m_Camera->GetTransform()->SetRotation(Vec3(0, 0, M_PI / 10));
	return m_Camera;
}

void Scene::LoadTestScene(int m)
{
	if (m == 0)
	{
		m_Objects.push_back(new Sphere(Vec3(0, 0, 50), 2, Color(255, 50, 170))); // small pink in back
		m_Objects.push_back(new Sphere(Vec3(5, 0, 15), 2, Color(70, 80, 255))); // reflective blue
		m_Objects.push_back(new Sphere(Vec3(-5, 0, 10), 2, Color(60, 255, 100))); // transparant green
		m_Objects.push_back(new Sphere(Vec3(-4, 2.9, 8.5), .5, Color(255, 255, 70))); // yellow in air
		m_Objects.push_back(new Sphere(Vec3(-3.2, -.25f, 10), .75, Color(140, 16, 140))); // purple in green
		m_Objects.push_back(new Sphere(Vec3(5, -2.5, 13), 1, Color(180, 120, 255))); // small purple
		m_Objects.push_back(new Sphere(Vec3(-2.5, -1, 45), 2, Color(124, 20, 77))); // burgundi
		m_Objects.push_back(new Sphere(Vec3(-3, -2.5, 10), 1.5f, Color(108, 92, 50))); // kaky
		m_Objects.back()->SetTexture("../Textures/test.jpg");
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
		//m_Objects.back()->SetReflective(.85f);
		//m_Objects.back()->SetBaseColor(Color(255, 215, 0));
		m_Objects.back()->SetTexture("../Textures/test.jpg");
		//m_Objects.push_back(new AABox(Vec3(-1, -3.5, 20), 5, 2, 5));
		m_Objects.push_back(new AABox(Vec3(5, 0, 20), 5, 6.5, 5, Color(29, 108, 129)));
		
		//AssimpModel* mo = new AssimpModel("../Meshes/teapot.FBX", { 0,-6,35 });
		//m_Objects.push_back(mo);

		//m_Objects.push_back(new AssimpModel("../Meshes/teapot.fbx", { 0.f,0.f,10.f }, {-(float)M_PI / 5.f,-(float)M_PI / 5.f,0.f}, {.1f,.1f,.1f}));
		//m_Objects.back()->SetTexture("../Textures/test.jpg");
		//m_Objects.push_back(new Triangle(PosNormVertex(Vec3(-5,-5,10), Vec3(0,0,-1)),
		//	PosNormVertex(Vec3(0, 5, 10), Vec3(0, 0, -1)),
		//	PosNormVertex(Vec3(5, -5, 10), Vec3(0, 0, -1))));
		//
		//m_Objects.push_back(new Triangle(PosNormVertex(Vec3(5, -5, 10), Vec3(0, 0, -1)),
		//	PosNormVertex(Vec3(10, 5, 10), Vec3(0, 0, -1)),
		//	PosNormVertex(Vec3(15, -5, 10), Vec3(0, 0, -1))));
		
		//m_Objects.push_back(new Triangle(PosNormVertex(Vec3(5, -5, 10), Vec3(0, 0, -1)),
		//		PosNormVertex(Vec3(10, 5, 10), Vec3(0, 0, -1)),
		//		PosNormVertex(Vec3(15, -5, 20), Vec3(0, 0, -1))));
		m_Lights.push_back(new Light({10,30,0}, {0,-1,0}, 2,2));
	}
	else if (m == 1)
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
			s = new Sphere(Vec3(cos(2 * M_PI / 15 * i) * 9, -3.5, sin(2 * M_PI / 15 * i) * 9 + 20), 1.5f, Color(rand() % 255, rand() % 255, rand() % 255));
			s->SetReflective(.8f);
			s->SetMetallic(true);
			//s->SetRefractive(1.5f);
			//s->SetTransparent(1.f);
			m_Objects.push_back(s);

			s = new Sphere(Vec3(cos(2 * M_PI / 15 * i) * 9, 4.5, sin(2 * M_PI / 15 * i) * 9 + 20), 1.5f, Color(rand() % 255, rand() % 255, rand() % 255));
			s->SetReflective(.8f);
			s->SetMetallic(true);
			//s->SetRefractive(1.5f);
			//s->SetTransparent(1.f);
			m_Objects.push_back(s);
		}
		s = new Sphere(Vec3(0, 0.5, 20), 3.f, Color(rand() % 255, rand() % 255, rand() % 255));
		s->SetReflective(.8f);
		s->SetMetallic(true);
		//s->SetRefractive(1.5f);
		//s->SetTransparent(1.f);
		m_Objects.push_back(s);

		m_Lights.push_back(new Light({ 4,5.5,20 }, { 0,-1,0 }, 1, 1));
	}
	else if (m == 2)
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
		
		m_Lights.push_back(new Light({ 0,0,40 }, { 0,-1,0 }, 1, 1));
	}
	else if (m == 3)
	{
		AssimpModel* mo = new AssimpModel("../Meshes/teapot.FBX", { 0,-5,35 }, Vec3(-M_PI / 10, -M_PI / 10, M_PI / 10));// , Vec3(1.f, 1.f, 1.f));
		mo->SetTexture("../Textures/test.jpg");
		m_Objects.push_back(mo);

		m_Lights.push_back(new Light({ 15,15,0 }, { 0,-1,0 }, 1, 1));
	}
}
