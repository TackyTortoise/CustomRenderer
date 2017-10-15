#include "AssimpModel.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include "Triangle.h"

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

bool AssimpModel::isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
{
	bool bbHit = m_BoundingBox->isHit(rayOrg, rayDir, hitDistance);
	if (!bbHit)
		return false;

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
				auto hp = rayOrg + rayDir * hitD;
				m_LastHitNormal = m_Triangles[i]->GetNormalOnHit(hp);
				m_LastHitUV = m_Triangles[i]->GetUvCoordOnHit(hp);
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
				Vec3 pos = m_Transform.GetTransformation().TransformVector(Vec3(mesh->mVertices[index]));

				if (pos.x < minimum.x)
					minimum.x = pos.x;
				if (pos.y < minimum.y)
					minimum.y = pos.y;
				if (pos.z < minimum.z)
					minimum.z = pos.z;

				if (pos.x > maximum.x)
					maximum.x = pos.x;
				if (pos.y > maximum.y)
					maximum.y = pos.y;
				if (pos.z > maximum.z)
					maximum.z = pos.z;

				Vec3 norm = m_Transform.GetRotationMatrix().TransformVector(Vec3(mesh->mNormals[index]));
				aiVector3D aiuv = mesh->mTextureCoords[0][index];
				Vec2 uv(-aiuv.x, -aiuv.y);
				m_Vertices.push_back(PosNormUVVertex(pos, norm, uv));
			}
		}
	}
	auto center = (minimum + maximum) / 2.f;
	auto width = maximum.x - minimum.x;
	auto height = maximum.y - minimum.y;
	auto depth = maximum.z - minimum.z;
	m_BoundingBox = new AABox(center, width, height, depth);
}

void AssimpModel::GenerateTriangles()
{
	for (int i = 0; i < m_Indices.size() - 2; i += 3)
	{
		auto t = new Triangle(m_Vertices[m_Indices[i]], m_Vertices[m_Indices[i + 1]], m_Vertices[m_Indices[i + 2]]);
		m_Triangles.push_back(t);
	}
}

void AssimpModel::ClearData()
{
	for (int i = 0; i < m_Triangles.size(); ++i)
	{
		delete m_Triangles[i];
		m_Triangles[i] = nullptr;
	}

	if (m_BoundingBox)
	{
		delete m_BoundingBox;
		m_BoundingBox = nullptr;
	}
}
