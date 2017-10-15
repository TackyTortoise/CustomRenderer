#include "TeapotScene.h"
#include "../Objects/CommonObjects.h"

TeapotScene::TeapotScene()
{
	AssimpModel* mo = new AssimpModel("../Meshes/teapot.FBX", { 0,-5,35 }, Vec3(-M_PI / 10, -M_PI / 10, M_PI / 10));// , Vec3(1.f, 1.f, 1.f));
	mo->SetTexture("../Textures/test.jpg");
	m_Objects.push_back(mo);

	m_Lights.push_back(new Light({ 15,15,0 }, { 0,-1,0 }, 1, 1));
}


TeapotScene::~TeapotScene()
{
}
