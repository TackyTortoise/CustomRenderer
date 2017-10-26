#include "GlassScene.h"
#include "../Objects/AssimpModel.h"
#include "../Objects/Plane.h"
#include "../Objects/Sphere.h"

GlassScene::GlassScene()
{
	/*auto sp = new Sphere(Vec3(0, 3.f, 20.f), 6.f, Color(255));
	sp->SetTransparent(.9f);
	sp->SetRefractive(1.52f);
	m_Objects.push_back(sp);/**/

	auto glassMesh = new AssimpModel("../Meshes/Glass/glass.fbx", Vec3(0,.2f,20), Vec3(-M_PI / 2, 0,0), Vec3(.25f));
	glassMesh->SetBaseColor(Color(255));
	glassMesh->SetTransparent(.98f);
	glassMesh->SetRefractive(1.52f);
	m_Objects.push_back(glassMesh);/**/

	auto waterMesh = new AssimpModel("../Meshes/Glass/water.fbx", Vec3(0, 2.7f, 20), Vec3(-M_PI / 2, 0, 0), Vec3(.25f));
	waterMesh->SetBaseColor(Color(0,0,255));
	waterMesh->SetTransparent(.98f);
	//waterMesh->SetRefractive(1.330f);
	m_Objects.push_back(waterMesh);

	Plane* pTest = new Plane();
	pTest->SetPosition({ 0, 0, 0 });
	pTest->SetTexture("../Textures/checker.jpg");
	//pTest->SetReflective(.4f);
	pTest->GetMaterial().SetTexScale(1.f);
	m_Objects.push_back(pTest);

	m_Lights.push_back(new Light({ 10,30,0 }, { 0,-1,0 }, 3, 3));

	m_Camera->GetTransform()->SetTranslation(Vec3(0, 20.f, 0));
	m_Camera->GetTransform()->SetRotation({ (float)M_PI / 5.f , 0.f, 0.f });
}

GlassScene::~GlassScene()
{
}
