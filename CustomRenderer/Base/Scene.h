#pragma once
#include <vector>
#include "../Objects/Object.h"
#include "Camera.h"
#include "Light.h"
#include "RenderSettings.h"

class Scene
{
public:
	Scene(const int n);
	~Scene();

	Camera* SetupCamera(const RenderSettings& rs);
	Camera* GetCamera() const { return m_Camera; }

	const std::vector<Object*>& GetObjectPointer() const { return m_Objects; }
	std::vector<Light*> GetLights() const { return m_Lights; }

private:
	void LoadTestScene(int num);

	Camera* m_Camera = nullptr;
	std::vector<Object*> m_Objects;
	std::vector<Light*> m_Lights;
	Color m_BackGroundColor = Color(0);
};

