#pragma once
#include "Scene.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	static SceneManager* GetInstance();
	static void Destroy();
	
	static Scene* GetActiveScene() { return m_ActiveScene; }
	void AddScene(Scene* scene);

	void NextScene();
	void PreviousScene();
	void SetActiveScene(unsigned index);

private:
	static SceneManager* m_Instance;
	static Scene* m_ActiveScene;
	std::vector<Scene*> m_Scenes;
	int m_CurrentSceneNumber = 0;
};

