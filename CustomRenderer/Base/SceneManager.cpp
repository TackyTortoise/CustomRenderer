#include "SceneManager.h"
#include "Renderer.h"

SceneManager* SceneManager::m_Instance = nullptr;

SceneManager::SceneManager()
{
}

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
	//make sure to not add duplicate scenes
	auto it = std::find(m_Scenes.begin(), m_Scenes.end(), scene);

	if (it == m_Scenes.end())
	{
		m_Scenes.push_back(scene);
	}
}

void SceneManager::NextScene()
{
	if (m_Scenes.size() <= 1)
		return;
	++m_CurrentSceneNumber;
	m_CurrentSceneNumber %= m_Scenes.size();
	SetActiveScene(m_CurrentSceneNumber);
}

void SceneManager::PreviousScene()
{
	if (m_Scenes.size() <= 1)
		return;
	--m_CurrentSceneNumber;
	if (m_CurrentSceneNumber < 0)
		m_CurrentSceneNumber = m_Scenes.size() - 1;
	SetActiveScene(m_CurrentSceneNumber);
}

void SceneManager::SetActiveScene(unsigned index)
{
	Renderer::GetInstance()->SetActiveScene(m_Scenes[index]);
}
