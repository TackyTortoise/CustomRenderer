#include "ReflectiveSpheresScene.h"
#include "../Objects/CommonObjects.h"

ReflectiveSpheresScene::ReflectiveSpheresScene()
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


ReflectiveSpheresScene::~ReflectiveSpheresScene()
{
}
