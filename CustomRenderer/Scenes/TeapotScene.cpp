#include "TeapotScene.h"
#include "../Objects/CommonObjects.h"

TeapotScene::TeapotScene()
{
	/*AssimpModel* mo = new AssimpModel("../Meshes/teapot.FBX", { 0,-5,35 }, Vec3(-M_PI / 10, -M_PI / 10, M_PI / 10));// , Vec3(1.f, 1.f, 1.f));
	mo->SetTexture("../Textures/test.jpg");
	m_Objects.push_back(mo);

	m_Lights.push_back(new Light({ 15,15,0 }, { 0,-1,0 }, 1, 1));*/

	AssimpModel* male = new AssimpModel("../Meshes/FatMale.FBX", Vec3(0,-1,2), Vec3(-M_PI / 2, M_PI + 0.1f, 0.f));
	male->SetTexture("../Textures/Fatmale_Color.jpg");
	male->SetNormalMap("../Textures/Prop_NormalMap.jpg");
	male->SetShininess(70.f);
	m_Objects.push_back(male);
	m_Lights.push_back(new Light({ 15,15,0 }, { 0,-1,0 }, 1, 1));
}


TeapotScene::~TeapotScene()
{
}
