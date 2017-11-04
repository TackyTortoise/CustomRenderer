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

	bool NextScene();
	bool PreviousScene();
	void SetActiveScene(unsigned index);

	static void LoadSceneLibrary();

private:
	static Scene* LoadSceneFromText(const char* path, bool autoAdd = true);

	static SceneManager* m_Instance;
	static Scene* m_ActiveScene;
	std::vector<Scene*> m_Scenes;
	int m_CurrentSceneNumber = 0;


};

