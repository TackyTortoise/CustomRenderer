#pragma once
#include <vector>
#include "../Objects/Object.h"
#include "Camera.h"
#include "Light.h"

class Scene
{
public:
	Scene();
	~Scene();

	void SetupCamera(const float fov, const float width, const float height);
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

