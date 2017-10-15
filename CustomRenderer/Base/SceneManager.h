#pragma once
#include "Scene.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	static SceneManager* GetInstance();
	static void Destroy();
	
	Scene* GetActiveScene() const { return m_ActiveScene; }
	void AddScene(Scene* scene);

	void NextScene();
	void PreviousScene();
	void SetActiveScene(unsigned index);

private:
	static SceneManager* m_Instance;
	Scene* m_ActiveScene = nullptr;
	std::vector<Scene*> m_Scenes;
	int m_CurrentSceneNumber = 0;
};

