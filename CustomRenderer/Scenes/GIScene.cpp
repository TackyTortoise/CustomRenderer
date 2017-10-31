#include "GIScene.h"
#include "../Objects/Plane.h"
#include "../Objects/Sphere.h"

GIScene::GIScene()
{
	//floor
	Plane* pTest = new Plane();
	pTest->SetPosition({ 0, -10, 0 });
	pTest->SetBaseColor(Color(150));
	//pTest->SetReflective(.3f);
	m_Objects.push_back(pTest);

	//roof
	pTest = new Plane();
	pTest->SetPosition({ 0,10,0 });
	pTest->SetNormal(Vec3(0, -1, 0));
	pTest->SetBaseColor(Color(150));
	m_Objects.push_back(pTest);

	//right
	pTest = new Plane;
	pTest->SetPosition({ 12,0,0 });
	pTest->SetNormal({ -1,0,0 });
	pTest->SetBaseColor(Color(0, 200, 0));
	m_Objects.push_back(pTest);

	//left
	pTest = new Plane;
	pTest->SetPosition({ -12,0,0 });
	pTest->SetNormal({ 1,0,0 });
	pTest->SetBaseColor(Color(200, 0, 0));
	m_Objects.push_back(pTest);

	//back
	pTest = new Plane;
	pTest->SetPosition({ 0,0,30 });
	pTest->SetNormal({ 0,0,-1 });
	pTest->SetBaseColor(Color(150));
	m_Objects.push_back(pTest);

	auto sp = new Sphere(Vec3(-7, -7, 25), 3.f, Color(230));
	m_Objects.push_back(sp);

	sp = new Sphere(Vec3(7, -7, 17), 3.f, Color(230));
	m_Objects.push_back(sp);

	sp = new Sphere(Vec3(0, -8, 25), 2.f, Color(255));
	sp->SetReflective(1.f);
	m_Objects.push_back(sp);

	m_Lights.push_back(new Light({ 0,8,25 }, { 0,-1,0 }, 3, 3));
	m_Lights.back()->SetColor(Color(250, 247, 212));
	MoveCamera(Vec3(0, -5, 0));
}


GIScene::~GIScene()
{
}
