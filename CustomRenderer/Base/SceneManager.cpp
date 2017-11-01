#include "SceneManager.h"
#include "Renderer.h"

SceneManager* SceneManager::m_Instance = nullptr;
Scene* SceneManager::m_ActiveScene = nullptr;

SceneManager::SceneManager(){}

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
	m_CurrentSceneNumber = index > m_Scenes.size() ? 0 : index;
	m_ActiveScene = m_Scenes[index];
	Renderer::GetInstance()->SetActiveScene(m_Scenes[index]);
}
