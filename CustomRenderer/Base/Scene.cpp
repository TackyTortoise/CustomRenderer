#include "Scene.h"
#include "../Objects/AssimpModel.h"
#include "Renderer.h"

Scene::Scene()
{
	SetupCamera(Renderer::GetRenderSettings());
}

Scene::~Scene()
{
	for(int i = 0; i < m_Objects.size(); ++i)
	{
		delete m_Objects[i];
		m_Objects[i] = nullptr;
	}

	for (int i = 0; i <m_Lights.size(); ++i)
	{
		delete m_Lights[i];
		m_Lights[i] = nullptr;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}
}

Camera* Scene::SetupCamera(const RenderSettings& rs)
{
	if (m_Camera != nullptr)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	m_Camera = new Camera(rs.cameraFOV, rs.screenWidth / static_cast<float>(rs.screenHeight));
	return m_Camera;
}

void Scene::MoveCamera(const Vec3& movement) const
{
	auto adjustedMove = m_Camera->GetTransform()->GetRotationMatrix().TransformVector(movement);
	m_Camera->GetTransform()->Move(adjustedMove);
}

void Scene::RotateCamera(const Vec3& rotation) const
{
	//Rotate camera
	m_Camera->GetTransform()->Rotate(rotation);
	//Update camera axes
	m_Camera->GetRight(true);
	m_Camera->GetUp(true);
}
