#include "TeapotScene.h"
#include "../Objects/CommonObjects.h"

TeapotScene::TeapotScene()
{
	/*AssimpModel* mo = new AssimpModel("../Meshes/teapot.FBX", { 0,-5,35 }, Vec3(-M_PI / 10, -M_PI / 10, M_PI / 10));// , Vec3(1.f, 1.f, 1.f));
	mo->SetTexture("../Textures/test.jpg");
	m_Objects.push_back(mo);*/

	/*AssimpModel* male = new AssimpModel("../Meshes/FatMale.FBX", Vec3(0,-1,2), Vec3(-M_PI / 2, M_PI + 0.1f, 0.f));
	male->SetTexture("../Textures/male.png");
	male->SetNormalMap("../Textures/Prop_NormalMap.jpg");
	male->SetShininess(70.f);
	m_Objects.push_back(male);*/

	auto bunny = new AssimpModel("../Meshes/stanford-bunny.fbx", Vec3(0,0,5), Vec3(-M_PI / 2, M_PI,0));
	bunny->SetBaseColor(Color(217, 144, 88));
	//bunny->SetReflective(.5f);
	bunny->SetMetallic(true);
	m_Objects.push_back(bunny);

	/*auto dragon = new AssimpModel("../Meshes/stanford-dragon.fbx", Vec3(0, -1.1f, 5.f), Vec3(0), Vec3(.3f));
	dragon->SetBaseColor(Color(218, 165, 32));
	dragon->SetReflective(.75f);
	m_Objects.push_back(dragon);*/

	auto plane = new Plane();
	plane->SetPosition({ 0,-1.1f,0 });
	plane->SetReflective(.75f);
	plane->SetTexture("../Textures/checker.jpg");
	//plane->SetNormalMap("../Textures/checkernormal.jpg");
	plane->GetMaterial().SetTexScale(.2f);
	m_Objects.push_back(plane);

	plane = new Plane();
	plane->SetPosition({ 0,100.f, 0 });
	plane->SetBaseColor(Color(128,0,0));
	m_Objects.push_back(plane);

	m_Lights.push_back(new Light({ 15,15,0 }, { 0,-1,0 }, 1, 1));

	m_Camera->GetTransform()->SetTranslation({ 0,2,0 });
	m_Camera->GetTransform()->SetRotation({ (float)M_PI / 10.f , 0.f, 0.f });
}

TeapotScene::~TeapotScene(){}
