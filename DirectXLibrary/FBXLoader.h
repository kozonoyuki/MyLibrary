// FBXLoader.h

#ifndef _FBX_LOADER_
#define _FBX_LOADER_

//#define _XM_NO_INTRINSICS_

#include <iostream>
#include <fbxsdk.h>
#include <directxmath.h>
#include <vector>
#include <map>
#include <algorithm>

#include <stdio.h>

struct ModelBoneWeight
{
	ModelBoneWeight() {};
	ModelBoneWeight(const ModelBoneWeight & model)
	{
		for (int i = 0; i < 4; ++i) boneIndex[i] = model.boneIndex[i];
		boneWeight = model.boneWeight;
	}
	uint8_t boneIndex[4];
	DirectX::XMFLOAT4 boneWeight;
};

struct ModelVertex
{
	
	ModelVertex() {};
	ModelVertex(const ModelVertex & model)
	{
		position = model.position;
		normal = model.normal;
		uv.clear();
		for (auto u : model.uv)
		{
			uv.push_back(u);
		}
		for (int i = 0; i < 4; ++i) boneIndex[i] = model.boneIndex[i];
		boneWeight = model.boneWeight;
	}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	std::vector<DirectX::XMFLOAT2> uv;
	uint8_t boneIndex[4];
	DirectX::XMFLOAT4 boneWeight;

	bool operator == (const ModelVertex& v) const
	{
		return std::memcmp(this, &v, sizeof(ModelVertex)) == 0;
	}
};

struct ModelMesh
{
	/*
	ModelMesh() {};
	ModelMesh(const ModelMesh & mesh)
	{
		nodeName = mesh.nodeName;
		materialNameList.clear();
		vertexList.clear();
		indexList.clear();
		boneNodeNameList.clear();
		invBoneBaseposeMatrixList.clear();

		for (auto m : mesh.materialNameList) materialNameList.push_back(m);
		for (auto i : mesh.indexList) indexList.push_back(i);
		for (auto v : mesh.vertexList) vertexList.push_back(v);
		invMeshBaseposeMatrix = mesh.invMeshBaseposeMatrix;
		if (mesh.boneNodeNameList.size() != 0)
			for (auto b : mesh.boneNodeNameList)
				boneNodeNameList.push_back(b);
		if (mesh.invBoneBaseposeMatrixList.size() != 0)
			for (auto i : mesh.invBoneBaseposeMatrixList)
				invBoneBaseposeMatrixList.push_back(i);
	}
	*/
	std::string nodeName;
	std::vector<std::string> materialNameList;
	std::vector<ModelVertex> vertexList;
	std::vector<uint16_t> indexList;
	std::vector<std::string> boneNodeNameList;
	std::vector<DirectX::XMMATRIX> invBoneBaseposeMatrixList;
	DirectX::XMMATRIX invMeshBaseposeMatrix;
};

struct ModelMaterial
{
	std::string materialName;

	std::string			diffuseTextureName;
	DirectX::XMFLOAT3	diffuseFactor;

	std::string			emissiveTextureName;
	DirectX::XMFLOAT3	emissiveFactor;

	std::string			specularTextureName;
	DirectX::XMFLOAT3	specularFactor;

	std::string			ambientTextureName;
	DirectX::XMFLOAT3	ambientFactor;
};

class FBXLoader
{
public:

	bool Initialize(const char* filepath);
	void Finalize();

	bool LoadAnimation(const char* filepath);

	int GetMeshCount() const
	{
		return (int)(this->meshList.size());
	}
	const ModelMesh& GetMesh(int index) const
	{
		return this->meshList[index];
	}

	int GetMaterialCount() const
	{
		return (int)this->materialList.size();
	}

	const int GetMaterialId(const std::string& materialName) const
	{
		return this->materialIdDictionary.at(materialName);
	}

	const ModelMaterial& GetMaterial(int materialId) const
	{
		return this->materialList[materialId];
	}

	float GetAnimationStartFrame() const
	{
		return this->animationStartFrame;
	}

	float GetAnimationEndFrame() const
	{
		return this->animationEndFrame;
	}

	void GetMeshMatrix(float frame, int meshId, DirectX::XMMATRIX* out_matrix) const;
	void GetBoneMatrix(float frame, int meshId, DirectX::XMMATRIX* out_matrixList, int matrixCount) const;

private:

	ModelMesh ParseMesh(FbxMesh* mesh);
	ModelMaterial ParseMaterial(FbxSurfaceMaterial* material);

private:

	FbxManager* sdkManager;
	FbxScene* fbxScene;

	std::vector<ModelMesh> meshList;
	std::vector<ModelMaterial> materialList;
	std::map<std::string, int> materialIdDictionary;
	std::map<std::string, int> nodeIdDictionary;

	FbxScene* fbxSceneAnimation;
	std::map<std::string, int> nodeIdDictionaryAnimation;
	float animationStartFrame;
	float animationEndFrame;
};

#endif