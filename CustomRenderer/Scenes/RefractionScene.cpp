#include "RefractionScene.h"
#include "../Objects/CommonObjects.h"

RefractionScene::RefractionScene()
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


RefractionScene::~RefractionScene()
{
}
