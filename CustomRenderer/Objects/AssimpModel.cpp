#include "AssimpModel.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include "Triangle.h"

AssimpModel::AssimpModel(const char* filePath, Vec3 pos)
{
	m_Position = pos;
	LoadModelFromFile(filePath);
	GenerateTriangles();
	m_Color = Color(255, 0, 255);
}

AssimpModel::~AssimpModel()
{
	for (int i = 0; i < m_Triangles.size();++i)
	{
		delete m_Triangles[i];
		m_Triangles[i] = nullptr;
	}
}

bool AssimpModel::isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
{
	float shortD = std::numeric_limits<float>::max();
	bool hit = false;
	for (int i = 0; i < m_Triangles.size(); ++i)
	{
		float hitD;
		if (m_Triangles[i]->isHit(rayOrg, rayDir, hitD))
		{
			if (hitD < shortD && hitD > 1e-5)
			{
				hit = true;
				shortD = hitD;
				m_LastHitNormal = m_Triangles[i]->GetNormalOnHit(rayOrg + rayDir * hitD);
			}
		}
	}
	hitDistance = shortD;
	return hit;
}

const Vec3 AssimpModel::GetNormalOnHit(Vec3 hitPosition) const
{
	return m_LastHitNormal;
}

void AssimpModel::LoadModelFromFile(const char* filePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (scene == nullptr)
	{
		std::cout << "Failed to load model " << filePath << std::endl;
	}

	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		int iMeshFaces = mesh->mNumFaces;
		for (int j = 0; j < iMeshFaces; ++j)
		{
			//aiVector3D p[3];
			//aiVector3D n[3];
			for (int k = 0; k < 3; ++k)
			{
				const aiFace& face = mesh->mFaces[j];
				unsigned index = face.mIndices[k];
				if (std::find(m_Indices.begin(), m_Indices.end(), index) != m_Indices.end())
				{
					m_Indices.push_back(index);
					continue;
				}

				m_Indices.push_back(index);
				aiVector3D pos = mesh->mVertices[index];
				aiVector3D norm = mesh->mNormals[index];
				//p[k] = pos;
				//n[k] = norm;
				m_Vertices.push_back(PosNormVertex(Vec3(pos) + m_Position, norm));
			}
			//m_Triangles.push_back(new Triangle(PosNormVertex(Vec3(p[0]) + m_Position, n[0]), PosNormVertex(Vec3(p[1]) + m_Position, n[1]), PosNormVertex(Vec3(p[2]) + m_Position, n[2])));
		}
	}
}

void AssimpModel::GenerateTriangles()
{
	for (int i = 0; i < m_Indices.size() - 2; i += 3)
	{
		m_Triangles.push_back(new Triangle(m_Vertices[m_Indices[i]], m_Vertices[m_Indices[i + 1]], m_Vertices[m_Indices[i + 2]]));
	}
}
