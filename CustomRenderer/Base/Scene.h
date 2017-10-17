#pragma once
#include <vector>
#include "../Objects/Object.h"
#include "Camera.h"
#include "Light.h"
#include "RenderSettings.h"

class Scene
{
public:
	Scene();
	~Scene();

	Camera* SetupCamera(const RenderSettings& rs);
	Camera* GetCamera() const { return m_Camera; }

	const std::vector<Object*>& GetObjectPointer() const { return m_Objects; }
	const std::vector<Light*>& GetLights() const { return m_Lights; }

protected:
	std::vector<Object*> m_Objects;
	std::vector<Light*> m_Lights;
	Camera* m_Camera = nullptr;
};

