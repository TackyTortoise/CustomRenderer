#include "TeapotScene.h"
#include "../Objects/CommonObjects.h"

TeapotScene::TeapotScene()
{
	/*AssimpModel* mo = new AssimpModel("../Data/Meshes/teapot.FBX", { 1,0,10 }, Vec3(-M_PI / 10, -M_PI / 10, M_PI / 10), Vec3(.1f));// , Vec3(1.f, 1.f, 1.f));
	mo->SetTexture("../Data/Textures/test.jpg");
	m_Objects.push_back(mo);/**/

	AssimpModel* male = new AssimpModel("../Data/Meshes/FatMale.FBX", Vec3(0,-1,4), Vec3(-M_PI / 2, M_PI + 0.1f, 0.f));
	male->SetTexture("../Data/Textures/male.png");
	male->SetNormalMap("../Data/Textures/Prop_NormalMap.jpg");
	male->SetShininess(70.f);
	m_Objects.push_back(male);/**/

	/*auto bunny = new AssimpModel("../Data/Meshes/stanford-bunny.fbx", Vec3(0,0.2,5), Vec3(-M_PI / 2, M_PI,0));
	bunny->SetBaseColor(Color(217, 144, 88));
	//bunny->SetReflective(.5f);
	bunny->SetMetallic(true);
	m_Objects.push_back(bunny);/**/

	auto chicken = new AssimpModel("../Data/Meshes/chicken.fbx", Vec3(1, 0.7f, 6), Vec3(-M_PI / 2, 0, 0), Vec3(.03f));
	chicken->SetTexture("../Data/Textures/chicken.jpg");
	chicken->SetShininess(5150.f);
	m_Objects.push_back(chicken);/**/

	auto caseModel = new AssimpModel("../Data/Meshes/case.fbx", Vec3(2.5, -1, 7), Vec3(-M_PI / 2, -M_PI * .8f,0), Vec3(.1f));
	caseModel->SetTexture("../Data/Textures/case.png");
	caseModel->GetMaterial().SetTexIsSRGB(false);
	caseModel->SetNormalMap("../Data/Textures/casenormal.png");
	m_Objects.push_back(caseModel);/**/

	/*auto dragon = new AssimpModel("../Data/Meshes/stanford-dragon.fbx", Vec3(0, -1.1f, 5.f), Vec3(0), Vec3(.3f));
	dragon->SetBaseColor(Color(218, 165, 32));
	dragon->SetReflective(.75f);
	m_Objects.push_back(dragon);*/

	auto plane = new Plane();
	plane->SetPosition({ 0,-1.f,0 });
	plane->SetReflective(.3f);
	//plane->SetRoughness(.05f);
	plane->SetTexture("../Data/Textures/checker.jpg");
	//plane->SetNormalMap("../Data/Textures/checkernormal.jpg");
	plane->GetMaterial().SetTexScale(2.f);
	m_Objects.push_back(plane);

	plane = new Plane();
	plane->SetPosition({ 0,100.f, 0 });
	plane->SetBaseColor(Color(100, 149, 237));
	plane->SetNormal(Vec3(0, -1, 0));
	m_Objects.push_back(plane);

	m_Lights.push_back(new Light({ 15,15,0 }, { 0,-1,0 }, 1, 1));

	m_Camera->GetTransform()->SetTranslation({ 0,2,0 });
	m_Camera->GetTransform()->SetRotation({ (float)M_PI / 10.f , 0.f, 0.f });

	m_Camera->EnableDOF(true);
	m_Camera->SetFocalDistance(4.f);
	m_Camera->SetAperture(0.03f);
}

TeapotScene::~TeapotScene(){}
