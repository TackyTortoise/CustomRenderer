#include "GlassScene.h"
#include "../Objects/CommonObjects.h"

GlassScene::GlassScene()
{
	auto glassMesh = new AssimpModel("../Data/Meshes/Glass/glass.fbx", Vec3(0, .2f,20), Vec3(-M_PI / 2, 0, 0), Vec3(.25f));
	glassMesh->SetMaterial("Glass");	
	m_Objects.push_back(glassMesh);/**/

	auto pencilMesh = new AssimpModel("../Data/Meshes/Glass/pencil.fbx", Vec3(0, .2f, 20), Vec3(-M_PI / 2, 0, 0), Vec3(.25f));
	pencilMesh->SetBaseColor(Color(128));
	m_Objects.push_back(pencilMesh);/**/

	auto waterMesh = new AssimpModel("../Data/Meshes/Glass/water.fbx", Vec3(0, .22f, 20), Vec3(-M_PI / 2, 0, 0), Vec3(.25f));
	waterMesh->SetMaterial("BlueWater");
	m_Objects.push_back(waterMesh);/**/

	Plane* pTest = new Plane();
	pTest->SetPosition({ 0, 0, 0 });
	pTest->SetTexture("../Data/Textures/checker.jpg");
	//pTest->SetReflective(.4f);
	pTest->GetMaterial()->SetTexScale(10.f);
	m_Objects.push_back(pTest);

	pTest = new Plane();
	pTest->SetPosition({ 0,50,0 });
	pTest->SetBaseColor(Color(255, 0, 255));
	m_Objects.push_back(pTest);

	m_Lights.push_back(new Light({ 10,30,0 }, { 0,-1,0 }, 3, 3));

	m_Camera->GetTransform()->SetTranslation(Vec3(0, 20.f, 0));
	m_Camera->GetTransform()->SetRotation({ (float)M_PI / 5.f , 0.f, 0.f });
}

GlassScene::~GlassScene()
{
}
