#include "AssimpModel.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include "Triangle.h"
#include "KDNode.h"

AssimpModel::AssimpModel(const char* filePath, const Vec3& pos, const Vec3& rotation, const Vec3& scale)
{
	m_Transform.SetFullTransform(pos, rotation, scale);
	LoadModelFromFile(filePath);
	GenerateTriangles();
	m_Material.color = Color(255, 0, 255);
}

AssimpModel::~AssimpModel()
{
	ClearData();
}

bool AssimpModel::IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo)
{
	//KD-Tree hit
	float max = std::numeric_limits<float>::max();
	return m_KDNode->IsHit(rayOrg, rayDir, hitInfo, max);
}

Vec3 AssimpModel::GetNormalOnHit(Vec3 hitPosition) const
{
	return m_LastHitNormal;
}

Vec2 AssimpModel::GetUvCoordOnHit(Vec3 hitPosition) const
{
	return m_LastHitUV;
}

void AssimpModel::LoadModelFromFile(const char* filePath)
{
	ClearData();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	Vec3 minimum(std::numeric_limits<float>::max()), maximum(std::numeric_limits<float>::min());

	if (scene == nullptr)
	{
		std::cout << "Failed to load model " << filePath << std::endl;
	}

	//load data
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		int iMeshFaces = mesh->mNumFaces;
		//For each triangle
		for (int j = 0; j < iMeshFaces; ++j)
		{
			//Get 3 vertices
			for (int k = 0; k < 3; ++k)
			{
				const aiFace& face = mesh->mFaces[j];
				unsigned index = face.mIndices[k];

				//If vertex does not exist yet, save index
				if (std::find(m_Indices.begin(), m_Indices.end(), index) != m_Indices.end())
				{
					m_Indices.push_back(index);
					continue;
				}

				//Save vertex data
				m_Indices.push_back(index);
				Vec3 pos = m_Transform.GetTransformation().TransformVector(Vec3(mesh->mVertices[index]));
				Vec3 norm = m_Transform.GetRotationMatrix().TransformVector(Vec3(mesh->mNormals[index]));
				aiVector3D aiuv = mesh->mTextureCoords[0][index];
				Vec2 uv(aiuv.x, 1 - aiuv.y);
				
				m_Vertices.push_back(PosNormUVVertex(pos, norm, uv));
			}
		}
	}
}

void AssimpModel::GenerateTriangles()
{
	//Create triangle list from index buffer
	m_Triangles.resize(m_Indices.size() / 3);
	for (int i = 0; i < m_Indices.size() - 2; i += 3)
	{
		auto t = new Triangle(m_Vertices[m_Indices[i]], m_Vertices[m_Indices[i + 1]], m_Vertices[m_Indices[i + 2]]);
		m_Triangles[i / 3] = t;
	}

	m_KDNode = m_KDNode->BuildTree(m_Triangles);
}

void AssimpModel::ClearData()
{
	for (int i = 0; i < m_Triangles.size(); ++i)
	{
		delete m_Triangles[i];
		m_Triangles[i] = nullptr;
	}
	m_Triangles.clear();

	if (m_BoundingBox)
	{
		delete m_BoundingBox;
		m_BoundingBox = nullptr;
	}

	if (m_KDNode)
	{
		delete m_KDNode;
		m_KDNode = nullptr;
	}
}
