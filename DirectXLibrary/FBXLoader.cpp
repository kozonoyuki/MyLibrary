// FBXLoader.cpp

#include "stdafx.h"
#include "FBXLoader.h"

std::vector<int> GetIndexList(FbxMesh* mesh)
{
	// あらかじめ三角形化してある
	auto polygonCount = mesh->GetPolygonCount();

	std::vector<int> indexList;
	indexList.reserve(polygonCount * 3);

	for (int i = 0; i < polygonCount; ++i)
	{
		indexList.push_back(mesh->GetPolygonVertex(i, 0));
		indexList.push_back(mesh->GetPolygonVertex(i, 1));
		indexList.push_back(mesh->GetPolygonVertex(i, 2));
	}

	return indexList;
}

std::vector<DirectX::XMFLOAT3> GetPositionList(FbxMesh* mesh, const std::vector<int>& indexList)
{
	std::vector<DirectX::XMFLOAT3> positionList;
	positionList.reserve(indexList.size());

	for (auto index : indexList)
	{
		FbxVector4 controlPoint = mesh->GetControlPointAt(index);

		positionList.push_back(DirectX::XMFLOAT3((float)controlPoint[0], (float)controlPoint[1], (float)controlPoint[2]));

		assert(controlPoint[3] == 0.0);
	}

	return positionList;
}

std::vector<DirectX::XMFLOAT3> GetNormalList(FbxMesh* mesh, const std::vector<int>& indexList)
{
	auto elementCount = mesh->GetElementNormalCount();

	// 法線要素が1のみ対応
	assert(elementCount == 1);

	auto element = mesh->GetElementNormal();
	auto mappingMode = element->GetMappingMode();
	auto referenceMode = element->GetReferenceMode();
	const auto& indexArray = element->GetIndexArray();
	const auto& directArray = element->GetDirectArray();

	// eDirectかeIndexDirectのみ対応
	assert((referenceMode == FbxGeometryElement::eDirect) || (referenceMode == FbxGeometryElement::eIndexToDirect));

	std::vector<DirectX::XMFLOAT3> normalList;
	normalList.reserve(indexList.size());

	if (mappingMode == FbxGeometryElement::eByControlPoint)
	{
		// コントロールポイントでマッピング
		for (auto index : indexList)
		{
			auto normalIndex = (referenceMode == FbxGeometryElement::eDirect)
				? index
				: indexArray.GetAt(index);
			auto normal = directArray.GetAt(normalIndex);
			normalList.push_back(DirectX::XMFLOAT3((float)normal[0], (float)normal[1], (float)normal[2]));

			// wは1.0のみ対応
			//assert(normal[3] == 1.0);
		}
	}
	else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
	{
		// ポリゴンバーってックスでマッピング
		auto indexByPolygonVertex = 0;
		auto polygonCount = mesh->GetPolygonCount();
		for (int i = 0; i < polygonCount; ++i)
		{
			auto polygonSize = mesh->GetPolygonSize(i);

			for (int j = 0; j < polygonSize; ++j)
			{
				auto normalIndex = (referenceMode == FbxGeometryElement::eDirect)
					? indexByPolygonVertex
					: indexArray.GetAt(indexByPolygonVertex);
				auto normal = directArray.GetAt(normalIndex);

				normalList.push_back(DirectX::XMFLOAT3((float)normal[0], (float)normal[1], (float)normal[2]));

				// wは1.0のみ対応
				//assert(normal[3] == 1.0);

				++indexByPolygonVertex;
			}
		}
	}
	else
	{
		// それ以外のマッピングモードには対応しない
		assert(false);
	}

	return normalList;
}

std::vector<DirectX::XMFLOAT2> GetUVList(FbxMesh* mesh, const std::vector<int>& indexList, int uvNo)
{
	std::vector<DirectX::XMFLOAT2> uvList;

	auto elementCount = mesh->GetElementUVCount();

	if (uvNo + 1 > elementCount)
	{
		return uvList;
	}

	auto element = mesh->GetElementUV(uvNo);
	auto mappingMode = element->GetMappingMode();
	auto referenceMode = element->GetReferenceMode();
	const auto& indexArray = element->GetIndexArray();
	const auto& directArray = element->GetDirectArray();

	// eDirectかeIndexDirectのみ対応
	assert((referenceMode == FbxGeometryElement::eDirect) || (referenceMode == FbxGeometryElement::eIndexToDirect));

	uvList.reserve(indexList.size());

	if (mappingMode == FbxGeometryElement::eByControlPoint)
	{
		// コントロールポイントでマッピング
		for (auto index : indexList)
		{
			auto uvIndex = (referenceMode == FbxGeometryElement::eDirect)
				? index
				: indexArray.GetAt(index);
			auto uv = directArray.GetAt(uvIndex);
			uvList.push_back(DirectX::XMFLOAT2((float)uv[0], (float)uv[1]));
		}
	}
	else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
	{
		// ポリゴンバーテックスでマッピング
		auto indexByPolygonVertex = 0;
		auto polygonCount = mesh->GetPolygonCount();
		for (int i = 0; i < polygonCount; ++i)
		{
			auto polygonSize = mesh->GetPolygonSize(i);

			for (int j = 0; j < polygonSize; ++j)
			{
				auto uvIndex = (referenceMode == FbxGeometryElement::eDirect)
					? indexByPolygonVertex
					: indexArray.GetAt(indexByPolygonVertex);
				auto uv = directArray.GetAt(uvIndex);

				uvList.push_back(DirectX::XMFLOAT2((float)uv[0], (float)uv[1]));

				++indexByPolygonVertex;
			}
		}
	}
	else
	{
		assert(false);
	}

	return uvList;
}

void GetWeight(FbxMesh* mesh, const std::vector<int>& indexList, std::vector<ModelBoneWeight>& boneWeightList, std::vector<std::string>& boneNodeNameList, std::vector<DirectX::XMMATRIX>& invBaseposeMatrixList)
{
	auto skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
	if (skinCount == 0)
	{
		return;	// ない
	}

	// スキンが1のみ対応
	assert(skinCount <= 1);

	auto controlPointsCount = mesh->GetControlPointsCount();
	using TmpWeight = std::pair<int, float>;
	std::vector<std::vector<TmpWeight>> tmpBoneWeightList(controlPointsCount);
	
	auto skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
	auto clusterCount = skin->GetClusterCount();
	
	for (int i = 0; i < clusterCount; ++i)
	{
		auto cluster = skin->GetCluster(i);

		// eNormalizeしか対応しない
		assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

		boneNodeNameList.push_back(cluster->GetLink()->GetName());

		auto indexCount = cluster->GetControlPointIndicesCount();
		auto indices = cluster->GetControlPointIndices();
		auto weights = cluster->GetControlPointWeights();

		for (int j = 0; j < indexCount; ++j)
		{
			auto controlPointIndex = indices[j];
			tmpBoneWeightList[controlPointIndex].push_back({ i, weights[j] });
		}

		auto baseposeMatrix = cluster->GetLink()->EvaluateGlobalTransform().Inverse();
		//auto bpm = (double*)baseposeMatrix;
		// ベースポーズの逆行列を作成しておく
		DirectX::XMMATRIX invBaseposeMatrix(
			(float)baseposeMatrix.Get(0, 0), (float)baseposeMatrix.Get(0, 1), (float)baseposeMatrix.Get(0, 2), (float)baseposeMatrix.Get(0, 3),
			(float)baseposeMatrix.Get(1, 0), (float)baseposeMatrix.Get(1, 1), (float)baseposeMatrix.Get(1, 2), (float)baseposeMatrix.Get(1, 3),
			(float)baseposeMatrix.Get(2, 0), (float)baseposeMatrix.Get(2, 1), (float)baseposeMatrix.Get(2, 2), (float)baseposeMatrix.Get(2, 3),
			(float)baseposeMatrix.Get(3, 0), (float)baseposeMatrix.Get(3, 1), (float)baseposeMatrix.Get(3, 2), (float)baseposeMatrix.Get(3, 3));

		invBaseposeMatrixList.push_back(invBaseposeMatrix);
	}

	
	// コントロールポイントに対応したウェイトを作成
	std::vector<ModelBoneWeight> boneWeightListControlPoints;
	for (auto& tmpBoneWeight : tmpBoneWeightList)
	{
		// ウェイトの大きさでソート
		std::sort(tmpBoneWeight.begin(), tmpBoneWeight.end(),
			[](const TmpWeight& weightA, const TmpWeight& weightB) { return weightA.second > weightB.second;  }
		);

		// 1頂点に4つより多くウェイトが割り振られているなら影響が少ないものは無視
		while (tmpBoneWeight.size() > 4)
		{
			tmpBoneWeight.pop_back();
		}

		// 4つに満たない場合はダミーを挿入
		while (tmpBoneWeight.size() < 4)
		{
			tmpBoneWeight.push_back({ 0, 0.0f });
		}

		ModelBoneWeight weight;
		float total = 0.0f;
		for (int i = 0; i < 4; ++i)
		{
			weight.boneIndex[i] = tmpBoneWeight[i].first;
			total += tmpBoneWeight[i].second;
		}
		weight.boneWeight.x = ((tmpBoneWeight[0].second != 0.0f) ? tmpBoneWeight[0].second / total : 0.0f);
		weight.boneWeight.y = ((tmpBoneWeight[1].second != 0.0f) ? tmpBoneWeight[1].second / total : 0.0f);
		weight.boneWeight.z = ((tmpBoneWeight[2].second != 0.0f) ? tmpBoneWeight[2].second / total : 0.0f);
		weight.boneWeight.w = ((tmpBoneWeight[3].second != 0.0f) ? tmpBoneWeight[3].second / total : 0.0f);

		boneWeightListControlPoints.push_back(weight);
	}

	// インデックスで展開
	for (auto index : indexList)
	{
		boneWeightList.push_back(boneWeightListControlPoints[index]);
	}
	
}

bool FBXLoader::Initialize(const char* filepath)
{
	this->sdkManager = FbxManager::Create();

	auto ios = FbxIOSettings::Create(this->sdkManager, IOSROOT);
	this->sdkManager->SetIOSettings(ios);

	auto importer = FbxImporter::Create(this->sdkManager, "");

	if (!importer->Initialize(filepath, -1, this->sdkManager->GetIOSettings()))
	{
		return false;
	}

	FbxAxisSystem::DirectX.ConvertScene(this->fbxScene);

	this->fbxScene = FbxScene::Create(this->sdkManager, "originalScene");
	importer->Import(this->fbxScene);
	importer->Destroy();

	// あらかじめポリゴンを全て三角形化しておく
	FbxGeometryConverter geometryConverter(this->sdkManager);
	geometryConverter.Triangulate(this->fbxScene, true);

	// ノード名からノードIDを取得できるように辞書に登録
	auto nodeCount = this->fbxScene->GetNodeCount();
	printf("nodeCount: %d\n", nodeCount);
	for (int i = 0; i < nodeCount; ++i)
	{
		auto fbxNode = this->fbxScene->GetNode(i);
		this->nodeIdDictionary.insert({ fbxNode->GetName(), i });
	}

	// シーンに含まれるマテリアルの解析
	auto materialCount = this->fbxScene->GetMaterialCount();
	this->materialList.reserve(materialCount);
	printf("materialCount: %d\n", materialCount);

	for (int i = 0; i < materialCount; ++i)
	{
		auto fbxMaterial = this->fbxScene->GetMaterial(i);
		auto modelMaterial = this->ParseMaterial(fbxMaterial);
		this->materialList.push_back(modelMaterial);
		this->materialIdDictionary.insert({ modelMaterial.materialName, i });
	}

	// シーンに含まれるメッシュの解析
	auto meshCount = this->fbxScene->GetMemberCount<FbxMesh>();
	this->meshList.reserve(meshCount);
	printf("meshCount: %d\n", meshCount);
	for (int i = 0; i < meshCount; ++i)
	{
		auto fbxMesh = this->fbxScene->GetMember<FbxMesh>(i);
		auto mesh = this->ParseMesh(fbxMesh);
		this->meshList.push_back(mesh);
	}

	return true;
}

void FBXLoader::Finalize()
{
	this->sdkManager->Destroy();

	this->meshList.clear();
	this->materialList.clear();
	this->materialIdDictionary.clear();
	this->nodeIdDictionary.clear();
}

bool FBXLoader::LoadAnimation(const char* filepath)
{
	auto importer = FbxImporter::Create(this->sdkManager, "");

	if (!importer->Initialize(filepath, -1, this->sdkManager->GetIOSettings()))
	{
		return false;
	}

	this->fbxSceneAnimation = FbxScene::Create(this->sdkManager, "animationScene");
	importer->Import(this->fbxSceneAnimation);

	// アニメーションフレーム数取得
	auto animStackCount = importer->GetAnimStackCount();
	printf("AnimationStackCount : %d\n", animStackCount);
	printf("ActiveStackName : %s\n", importer->GetActiveAnimStackName());
	assert(animStackCount == 1);
	auto takeInfo = importer->GetTakeInfo(0);

	auto importOffset = takeInfo->mImportOffset;
	auto startTime = takeInfo->mLocalTimeSpan.GetStart();
	auto stopTime = takeInfo->mLocalTimeSpan.GetStop();

	this->animationStartFrame = (importOffset.Get() + startTime.Get()) / FbxTime::GetOneFrameValue(FbxTime::eFrames60);
	this->animationEndFrame = (importOffset.Get() + stopTime.Get()) / FbxTime::GetOneFrameValue(FbxTime::eFrames60);
	importer->Destroy();

	// ノード名からノードIDを取得できるように辞書に登録
	auto nodeCount = this->fbxSceneAnimation->GetNodeCount();
	printf("animationNodeCount: %d\n", nodeCount);
	for (int i = 0; i < nodeCount; ++i)
	{
		auto fbxNode = this->fbxSceneAnimation->GetNode(i);
		this->nodeIdDictionaryAnimation.insert({ fbxNode->GetName(), i });
	}

	return true;
}

ModelMesh FBXLoader::ParseMesh(FbxMesh* mesh)
{
	assert(mesh != nullptr);
	
	auto node = mesh->GetNode();

	// マテリアルが1のものしか対応しない 対応済み
	// assert(node->GetMaterialCount() == 1);

	ModelMesh modelMesh;
	modelMesh.nodeName = node->GetName();

	auto materialCount = node->GetMaterialCount();
	modelMesh.materialNameList.reserve(materialCount);
	
	for (int i = 0; i < materialCount; ++i)
	{
		modelMesh.materialNameList.push_back(node->GetMaterial(i)->GetName());
		//printf(">> mesh: %s, %s\n", modelMesh.nodeName.c_str(), modelMesh.materialNameList[i].c_str());
	}
	
	// ベースポーズの逆行列を作成しておく
	auto baseposeMatrix = node->EvaluateGlobalTransform().Inverse();
	//auto baseposeMatrixPtr = (double*)baseposeMatrix;
	modelMesh.invMeshBaseposeMatrix = DirectX::XMMATRIX
		(
		(float)baseposeMatrix.Get(0, 0), (float)baseposeMatrix.Get(0, 1), (float)baseposeMatrix.Get(0, 2), (float)baseposeMatrix.Get(0, 3),
		(float)baseposeMatrix.Get(1, 0), (float)baseposeMatrix.Get(1, 1), (float)baseposeMatrix.Get(1, 2), (float)baseposeMatrix.Get(1, 3),
		(float)baseposeMatrix.Get(2, 0), (float)baseposeMatrix.Get(2, 1), (float)baseposeMatrix.Get(2, 2), (float)baseposeMatrix.Get(2, 3),
		(float)baseposeMatrix.Get(3, 0), (float)baseposeMatrix.Get(3, 1), (float)baseposeMatrix.Get(3, 2), (float)baseposeMatrix.Get(3, 3)
		);
	
	// インデックス取得
	auto indexList = GetIndexList(mesh);

	// 全て展開した状態の頂点取得
	auto positionList = GetPositionList(mesh, indexList);
	auto normalList = GetNormalList(mesh, indexList);

	std::vector<std::vector<DirectX::XMFLOAT2>> uvLists;
	for (int i = 0; i < materialCount; ++i)
	{
		auto uvList = GetUVList(mesh, indexList, i);
		// マテリアルカウントが2でも、GetUVList内でUVが1のみのことがある
		if (i > 0 && uvList.size() == 0) {
			uvLists.push_back(uvLists[0]);
		}
		uvLists.push_back(uvList);
	}
	
	std::vector<ModelBoneWeight> boneWeightList;
	GetWeight(mesh, indexList, boneWeightList, modelMesh.boneNodeNameList, modelMesh.invBoneBaseposeMatrixList);
	
	// 念のためサイズチェック
	assert(indexList.size() == positionList.size());
	assert(indexList.size() == normalList.size());
	for (int i = 0; i < materialCount; ++i)
	{
		assert(indexList.size() == uvLists[i].size());
	}
	assert(indexList.size() == boneWeightList.size() || (boneWeightList.size() == 0));
	
	
	// 頂点をインタリーブドアレイに
	std::vector<ModelVertex> modelVertexList;
	modelVertexList.reserve(indexList.size());

	for (int i = 0; i < indexList.size(); ++i)
	{
		ModelVertex vertex;
		vertex.position = positionList[i];
		vertex.normal = normalList[i];
		for (int j = 0; j < materialCount; ++j)
		{
			vertex.uv.push_back(
				(uvLists[j].size() == 0)
				? DirectX::XMFLOAT2(0.0f, 0.0f)
				: uvLists[j][i]
				);
		}

		if (boneWeightList.size() > 0)
		{
			for (int j = 0; j < 4; ++j)
			{
				vertex.boneIndex[j] = boneWeightList[i].boneIndex[j];
			}
			vertex.boneWeight = boneWeightList[i].boneWeight;
		}
		else
		{
			for (int j = 0; j < 4; ++j)
			{
				vertex.boneIndex[j] = 0;
			}
			vertex.boneWeight = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
		}
		//printf("weight[%d]: %f, %f, %f, %f\n", i, vertex.boneWeight.x, vertex.boneWeight.y, vertex.boneWeight.z, vertex.boneWeight.w);
		modelVertexList.push_back(vertex);
	}

	// 重複頂点を除いてインデックス作成
	auto& modelVertexListOpt = modelMesh.vertexList;
	modelVertexListOpt.reserve(modelVertexList.size());

	auto& modelIndexList = modelMesh.indexList;
	modelIndexList.reserve(indexList.size());

	for (auto& vertex : modelVertexList)
	{
		auto it = std::find(modelVertexListOpt.begin(), modelVertexListOpt.end(), vertex);
		if (it == modelVertexListOpt.end())
		{
			// 重複していない
			modelIndexList.push_back(modelVertexListOpt.size());
			modelVertexListOpt.push_back(vertex);
		}
		else
		{
			// 重複している
			auto index = std::distance(modelVertexListOpt.begin(), it);
			modelIndexList.push_back(index);
		}
	}
	
	//printf("Opt: %lu -> %lu\n", modelVertexList.size(), modelVertexListOpt.size());
	//delete[] baseposeMatrixPtr;
	return modelMesh;
} 

ModelMaterial FBXLoader::ParseMaterial(FbxSurfaceMaterial* material)
{
	ModelMaterial modelMaterial;
	modelMaterial.materialName = material->GetName();

	printf(">> material: %s\n", modelMaterial.materialName.c_str());

	auto SetMaterialFactorFunc = [&](static const char * materialLight, static const char* materialLightFactor) -> DirectX::XMFLOAT3
	{
		FbxProperty property = material->FindProperty(materialLight);
		FbxProperty propertyFactor = material->FindProperty(materialLightFactor);
		if (property.IsValid() && propertyFactor.IsValid())
		{
			FbxDouble3 fres = property.Get<FbxDouble3>();
			double factor = propertyFactor.Get<FbxDouble>();
			if (factor != 1)
			{
				fres[0] *= factor;
				fres[1] *= factor;
				fres[2] *= factor;
				return DirectX::XMFLOAT3((float)fres[0], (float)fres[1], (float)fres[2]);
			}
			return DirectX::XMFLOAT3((float)fres[0], (float)fres[1], (float)fres[2]);
		}
		return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	};

	auto SetMaterialFunc = [&](static const char * materialLight) -> const char*
	{
		FbxProperty property = material->FindProperty(materialLight);
		if (property.IsValid())
		{
			// テクスチャ数
			int TextureCount = property.GetSrcObjectCount<FbxFileTexture>();
			printf("TextureCount: %d\n", TextureCount);
			if (TextureCount >= 1)
			{
				FbxFileTexture * fileTexture = property.GetSrcObject<FbxFileTexture>(0);
				return fileTexture->GetFileName();
			}
			else
			{
				return "";
			}
		}
		return "";
	};

	// ライティングマテリアルのパラメータを設定
	modelMaterial.diffuseFactor = SetMaterialFactorFunc(FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	modelMaterial.ambientFactor = SetMaterialFactorFunc(FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	modelMaterial.emissiveFactor = SetMaterialFactorFunc(FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
	modelMaterial.specularFactor = SetMaterialFactorFunc(FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);

	// ライティングマテリアルのテクスチャパスを設定
	modelMaterial.diffuseTextureName = SetMaterialFunc(FbxSurfaceMaterial::sDiffuse);
	modelMaterial.ambientTextureName = SetMaterialFunc(FbxSurfaceMaterial::sAmbient);
	modelMaterial.emissiveTextureName = SetMaterialFunc(FbxSurfaceMaterial::sEmissive);
	modelMaterial.specularTextureName = SetMaterialFunc(FbxSurfaceMaterial::sSpecular);

	// CGFXのみ対応 元ソース
	/*
	auto implementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_CGFX);
	assert(implementation != nullptr);

	auto rootTable = implementation->GetRootTable();
	auto entryCount = rootTable->GetEntryCount();

	for (int i = 0; i < entryCount; ++i)
	{
	auto entry = rootTable->GetEntry(i);

	auto fbxProperty = material->FindPropertyHierarchical(entry.GetSource());
	if (!fbxProperty.IsValid())
	{
	fbxProperty = material->RootProperty.FindHierarchical(entry.GetSource());
	}

	auto textureCount = fbxProperty.GetSrcObjectCount<FbxTexture>();
	if (textureCount > 0)
	{
	std::string src = entry.GetSource();

	for (int j = 0; j < fbxProperty.GetSrcObjectCount<FbxFileTexture>(); ++j)
	{
	auto tex = fbxProperty.GetSrcObject<FbxFileTexture>(j);
	std::string texName = tex->GetFileName();
	texName = texName.substr(texName.find_last_of('/') + 1);

	if (src == "Maya|DiffuseTexture")
	{
	modelMaterial.diffuseTextureName = texName;
	}
	else if (src == "Maya|NormalTexture")
	{
	modelMaterial.normalTextureName = texName;
	}
	else if (src == "Maya|SpecularTexture")
	{
	modelMaterial.specularTextureName = texName;
	}
	else if (src == "Maya|FalloffTexture")
	{
	modelMaterial.falloffTextureName = texName;
	}
	else if (src == "Maya|ReflectionMapTexture")
	{
	modelMaterial.reflectionMapTextureName = texName;
	}
	}
	}
	}
	*/
	printf("diffuseTexture: %s (%f,%f,%f)\n", modelMaterial.diffuseTextureName.c_str(), modelMaterial.diffuseFactor.x, modelMaterial.diffuseFactor.y, modelMaterial.diffuseFactor.z);
	printf("emissiveTexture: %s (%f,%f,%f)\n", modelMaterial.emissiveTextureName.c_str(), modelMaterial.emissiveFactor.x, modelMaterial.emissiveFactor.y, modelMaterial.emissiveFactor.z);
	printf("specularTexture: %s (%f,%f,%f)\n", modelMaterial.specularTextureName.c_str(), modelMaterial.specularFactor.x, modelMaterial.specularFactor.y, modelMaterial.specularFactor.z);
	printf("ambientTexture: %s (%f,%f,%f)\n", modelMaterial.ambientTextureName.c_str(), modelMaterial.ambientFactor.x, modelMaterial.ambientFactor.y, modelMaterial.ambientFactor.z);

	return modelMaterial;
}

void FBXLoader::GetMeshMatrix(float frame, int meshId, DirectX::XMMATRIX* out_matrix) const
{
	auto& modelMesh = this->meshList[meshId];

	auto it = this->nodeIdDictionaryAnimation.find(modelMesh.nodeName);
	if (it == this->nodeIdDictionaryAnimation.end())
	{
		*out_matrix = DirectX::XMMatrixIdentity();
		return;
	}

	auto meshNodeId = it->second;
	auto meshNode = this->fbxSceneAnimation->GetNode(meshNodeId);

	FbxTime time;
	time.Set(FbxTime::GetOneFrameValue(FbxTime::eFrames60) * frame);

	auto& meshMatrix = meshNode->EvaluateGlobalTransform(time);

	auto meshMatrixPtr = (double*)meshMatrix;
	*out_matrix = DirectX::XMMATRIX
		(
		(float)meshMatrixPtr[0], (float)meshMatrixPtr[1], (float)meshMatrixPtr[2], (float)meshMatrixPtr[3],
		(float)meshMatrixPtr[4], (float)meshMatrixPtr[5], (float)meshMatrixPtr[6], (float)meshMatrixPtr[7],
		(float)meshMatrixPtr[8], (float)meshMatrixPtr[9], (float)meshMatrixPtr[10], (float)meshMatrixPtr[11],
		(float)meshMatrixPtr[12], (float)meshMatrixPtr[13], (float)meshMatrixPtr[14], (float)meshMatrixPtr[15]
		);

	*out_matrix = DirectX::XMMatrixMultiply(*out_matrix, modelMesh.invMeshBaseposeMatrix);
}

void FBXLoader::GetBoneMatrix(float frame, int meshId, DirectX::XMMATRIX* out_matrixList, int matrixCount) const
{
	auto& modelMesh = this->meshList[meshId];

	if (modelMesh.boneNodeNameList.size() == 0)
	{
		out_matrixList[0] = DirectX::XMMatrixIdentity();
		return;
	}

	assert(modelMesh.boneNodeNameList.size() <= matrixCount);

	FbxTime time;
	time.Set(FbxTime::GetOneFrameValue(FbxTime::eFrames60) * frame);

	for (int i = 0; i < modelMesh.boneNodeNameList.size(); ++i)
	{
		auto& boneNodeName = modelMesh.boneNodeNameList[i];
		auto boneNodeId = this->nodeIdDictionaryAnimation.at(boneNodeName);
		auto boneNode = this->fbxSceneAnimation->GetNode(boneNodeId);

		auto& boneMatrix = boneNode->EvaluateGlobalTransform(time);
		auto& out_matrix = out_matrixList[i];

		auto boneMatrixPtr = (double*)boneMatrix;
		out_matrix = DirectX::XMMATRIX
			(
			(float)boneMatrixPtr[0], (float)boneMatrixPtr[1], (float)boneMatrixPtr[2], (float)boneMatrixPtr[3],
			(float)boneMatrixPtr[4], (float)boneMatrixPtr[5], (float)boneMatrixPtr[6], (float)boneMatrixPtr[7],
			(float)boneMatrixPtr[8], (float)boneMatrixPtr[9], (float)boneMatrixPtr[10], (float)boneMatrixPtr[11],
			(float)boneMatrixPtr[12], (float)boneMatrixPtr[13], (float)boneMatrixPtr[14], (float)boneMatrixPtr[15]
			);

		out_matrix = DirectX::XMMatrixMultiply(out_matrix, modelMesh.invBoneBaseposeMatrixList[i]);
	}
}