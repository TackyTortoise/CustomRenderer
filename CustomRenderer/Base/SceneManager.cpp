#include "SceneManager.h"
#include "Renderer.h"
#include "TextLoader.h"
#include "../Objects/CommonObjects.h"
#include "../Scenes/TeapotScene.h"
#include "../Scenes/GIScene.h"
#include "../Scenes/GlassScene.h"
#include "../Scenes/RefractionScene.h"
#include "../Scenes/ReflectiveSpheresScene.h"
#include "../Scenes/TestScene.h"

SceneManager* SceneManager::m_Instance = nullptr;
Scene* SceneManager::m_ActiveScene = nullptr;

SceneManager::SceneManager() {}

SceneManager::~SceneManager()
{
	for (int i = 0; i < m_Scenes.size(); ++i)
	{
		delete m_Scenes[i];
		m_Scenes[i] = nullptr;
	}
	m_Scenes.clear();
}

SceneManager* SceneManager::GetInstance()
{
	if (!m_Instance)
	{
		m_Instance = new SceneManager();
	}
	return m_Instance;
}

void SceneManager::Destroy()
{
	if (!m_Instance)
		return;
	delete m_Instance;
	m_Instance = nullptr;
}

void SceneManager::AddScene(Scene* scene)
{
	//Make sure to not add duplicate scenes
	auto it = std::find(m_Scenes.begin(), m_Scenes.end(), scene);

	if (it == m_Scenes.end())
	{
		m_Scenes.push_back(scene);
	}
}

bool SceneManager::NextScene()
{
	if (m_Scenes.size() <= 1)
		return false;
	++m_CurrentSceneNumber;
	m_CurrentSceneNumber %= m_Scenes.size();
	SetActiveScene(m_CurrentSceneNumber);
	return true;
}

bool SceneManager::PreviousScene()
{
	if (m_Scenes.size() <= 1)
		return false;
	--m_CurrentSceneNumber;
	if (m_CurrentSceneNumber < 0)
		m_CurrentSceneNumber = m_Scenes.size() - 1;
	SetActiveScene(m_CurrentSceneNumber);
	return true;
}

void SceneManager::SetActiveScene(unsigned index)
{
	int newInd = index > m_Scenes.size() ? 0 : index;
	if (m_Scenes[index]->m_Lights.size() < 1)
	{
		std::cout << "No lights in scene you're trying to load" << std::endl;
	}
	m_CurrentSceneNumber = newInd;
	m_ActiveScene = m_Scenes[newInd];
	Renderer::GetInstance()->SetActiveScene(m_Scenes[newInd]);
}

void SceneManager::LoadSceneLibrary()
{
	auto s = TextLoader::TxtFileToString("../Data/SceneLibrary.txt");
	auto enableTestScenes = TextLoader::FindIntValueInString(s, "enabletestscenes");
	auto enableCustomScenes = TextLoader::FindIntValueInString(s, "loadscenesbelow");

	if (enableTestScenes > 0)
	{
		/*GetInstance()->AddScene(new TestScene());
		GetInstance()->AddScene(new ReflectiveSpheresScene());
		GetInstance()->AddScene(new RefractionScene());
		GetInstance()->AddScene(new GlassScene());*/
		GetInstance()->AddScene(new GIScene());/*
		GetInstance()->AddScene(new TeapotScene());*/
	}

	if (enableCustomScenes > 0)
	{
		auto sInd = s.find("<scene>");
		while (sInd != std::string::npos)
		{
			auto sEnd = s.find("</scene>");
			if (sEnd == std::string::npos)
			{
				std::cout << "Scene definition has no end delimiter" << std::endl;
				sInd = s.find("<scene>", sInd + 1);
				continue;
			}

			auto sceneText = s.substr(sInd + 7, sEnd - sInd - 7);
			auto fileName = TextLoader::FindStringValueInString(sceneText, "file");
			auto path = "../Data/Scenes/" + fileName;
			auto scene = GetInstance()->LoadSceneFromText(path.c_str());
			GetInstance()->AddScene(scene);

			sInd = s.find("<scene>", sInd + 1);
		}
	}

	if (GetInstance()->m_Scenes.size() == 0)
	{
		std::cout << "Failed to load scenes from scene library, using testscene" << std::endl;
		GetInstance()->AddScene(new TestScene());
	}
}

Scene* SceneManager::LoadSceneFromText(const char* path, bool autoAdd)
{
	auto s = TextLoader::TxtFileToString(path);

	Scene* newScene = new Scene();

	//Load Planes
	auto pInd = s.find("<plane>");
	while (pInd != std::string::npos)
	{
		auto pEnd = s.find("</plane>", pInd);
		if (pEnd == std::string::npos)
		{
			std::cout << "Plane definition has no end delimiter in scenefile " << path << std::endl;
			pInd = s.find("<plane>", pInd + 1);
			continue;
		}
		auto planeText = s.substr(pInd + 7, pEnd - pInd - 7);

		auto pos = TextLoader::FindVectorValueInString(planeText, "position");
		auto norm = TextLoader::FindVectorValueInString(planeText, "normal");
		auto mat = TextLoader::FindStringValueInString(planeText, "material");

		auto plane = new Plane();
		plane->SetPosition(pos);
		plane->SetNormal(norm);
		plane->SetMaterial(mat.c_str());
		newScene->m_Objects.push_back(plane);

		pInd = s.find("<plane>", pInd + 1);
	}

	//Load Spheres
	auto sInd = s.find("<sphere>");
	while (sInd != std::string::npos)
	{
		auto sEnd = s.find("</sphere>", sInd);
		if (sEnd == std::string::npos)
		{
			std::cout << "Sphere definition has no end delimiter in scenefile " << path << std::endl;
			sInd = s.find("<sphere>", sInd + 1);
			continue;
		}

		auto sphereText = s.substr(sInd + 8, sEnd - sInd - 8);

		auto pos = TextLoader::FindVectorValueInString(sphereText, "position");
		auto r = TextLoader::FindFloatValueInString(sphereText, "radius");
		auto mat = TextLoader::FindStringValueInString(sphereText, "material");

		auto sphere = new Sphere(pos, r, Color(255, 0, 255));
		sphere->SetMaterial(mat.c_str());
		newScene->m_Objects.push_back(sphere);

		sInd = s.find("<sphere>", sInd + 1);
	}

	//Load Boxes
	auto bInd = s.find("<box>");
	while (bInd != std::string::npos)
	{
		auto bEnd = s.find("</box>", bInd);
		if (bEnd == std::string::npos)
		{
			std::cout << "Box definition has no end delimiter in scenefile " << path << std::endl;
			bInd = s.find("<box>", bInd + 1);
			continue;
		}

		auto boxText = s.substr(bInd + 5, bEnd - bInd - 5);

		auto pos = TextLoader::FindVectorValueInString(boxText, "position");
		auto width = TextLoader::FindFloatValueInString(boxText, "width");
		auto height = TextLoader::FindFloatValueInString(boxText, "height");
		auto depth = TextLoader::FindFloatValueInString(boxText, "depth");
		auto mat = TextLoader::FindStringValueInString(boxText, "material");

		auto box = new AABox(pos, width, height, depth, Color(255, 0, 255));
		box->SetMaterial(mat.c_str());
		newScene->m_Objects.push_back(box);

		bInd = s.find("<box>", bInd + 1);
	}

	//Load Models
	auto mInd = s.find("<model>");
	while (mInd != std::string::npos)
	{
		auto mEnd = s.find("</model>", mInd);
		if (mEnd == std::string::npos)
		{
			std::cout << "Model definition has no end delimiter in scenefile " << path << std::endl;
			mInd = s.find("<model>", mInd + 1);
			continue;
		}

		auto modelText = s.substr(mInd + 7, mEnd - mInd - 7);

		auto mesh = TextLoader::FindStringValueInString(modelText, "mesh");
		auto pos = TextLoader::FindVectorValueInString(modelText, "position");
		auto rot = TextLoader::FindVectorValueInString(modelText, "rotation");
		rot.x = Math::DegToRad(rot.x);
		rot.y = Math::DegToRad(rot.y);
		rot.z = Math::DegToRad(rot.z);
		auto scale = TextLoader::FindFloatValueInString(modelText, "scale");
		auto mat = TextLoader::FindStringValueInString(modelText, "material");

		auto mPath = "../Data/Meshes/" + mesh;
		auto model = new AssimpModel(mPath.c_str(), pos, rot, Vec3(scale));
		model->SetMaterial(mat.c_str());
		newScene->m_Objects.push_back(model);

		mInd = s.find("<model>", mInd + 1);
	}

	//Load Lights
	auto lInd = s.find("<light>");
	while (lInd != std::string::npos)
	{
		auto lEnd = s.find("</light>", lInd);
		if (lEnd == std::string::npos)
		{
			std::cout << "Light definition has no end delimiter in scenefile " << path << std::endl;
			lInd = s.find("<light>", lInd + 1);
			continue;
		}

		auto lightText = s.substr(lInd + 7, lEnd - lInd - 7);

		auto pos = TextLoader::FindVectorValueInString(lightText, "position");
		auto norm = TextLoader::FindVectorValueInString(lightText, "normal");
		auto width = TextLoader::FindFloatValueInString(lightText, "width");
		auto height = TextLoader::FindFloatValueInString(lightText, "height");
		auto col = TextLoader::FindColorValueInString(lightText, "color");

		auto light = new Light(pos, norm, width, height, col.ToCharColor());
		newScene->m_Lights.push_back(light);

		lInd = s.find("<light>", lInd + 1);
	}

	//Load Camera Info
	auto cInd = s.find("<camera>");
	if (cInd != std::string::npos)
	{
		auto cEnd = s.find("</camera>", cInd);
		if (cEnd == std::string::npos)
		{
			std::cout << "camera definition has no end delimiter in scenefile " << path << std::endl;
		}

		else
		{
			auto camText = s.substr(cInd + 7, cEnd - cInd - 7);

			auto pos = TextLoader::FindVectorValueInString(camText, "position");
			auto rot = TextLoader::FindVectorValueInString(camText, "rotation");
			rot.x = Math::DegToRad(rot.x);
			rot.y = Math::DegToRad(rot.y);
			rot.z = Math::DegToRad(rot.z);
			auto dof = TextLoader::FindIntValueInString(camText, "enabledof");
			auto fd = TextLoader::FindFloatValueInString(camText, "focaldistance");
			auto aperture = TextLoader::FindFloatValueInString(camText, "aperture");

			newScene->MoveCamera(pos);
			if (rot.x != -1)
				newScene->RotateCamera(rot);
			newScene->m_Camera->EnableDOF(dof > 0);
			newScene->m_Camera->SetFocalDistance(fd);
			newScene->m_Camera->SetAperture(aperture);
		}
	}

	if (autoAdd)
		GetInstance()->AddScene(newScene);

	return newScene;
}
