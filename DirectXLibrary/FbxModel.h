//--------------------------------------------------------------------------------------
// FbxModel.h
// FbxSdkを用いたモデルクラス
//--------------------------------------------------------------------------------------

#ifndef _FBX_MODEL_
#define _FBX_MODEL_

#include "CommonModel.h"
#include "fbxsdk.h"
#include "FBXLoader.h"
#include "FBXMesh.h"

class FbxModel : public CommonModel{
private:

	FBXLoader* fbxloader = nullptr;

	std::vector<FBXMesh*> fbxmesh;

	typedef enum
	{
		eUnknown,
		eNull,
		eMarker,
		eSkeleton,
		eMesh,
		eNurbs,
		ePatch,
		eCamera,
		eCameraStereo,
		eCameraSwitcher,
		eLight,
		eOpticalReference,
		eOpticalMarker,
		eNurbsCurve,
		eTrimNurbsSurface,
		eBoundary,
		eNurbsSurface,
		eShape,
		eLODGroup,
		eSubDiv,
		eCachedEffect,
		eLine
	} AttribType;

	float light_z;

public:
	std::string m_filename;
	std::string m_texturename;
	std::string m_shadername;

public:
	FbxModel();
	// モデルデータをバッファに格納する
	HRESULT LoadModel(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, std::string filename, std::string shadername);
	// モデルデータをシェーダに送信する
	HRESULT DrawModel(ID3D11DeviceContext* pd3dDeviceContext, const DirectX::XMMATRIX & view, const DirectX::XMMATRIX & projection, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, bool shadowmap, DirectX::XMFLOAT4 eyepos);

	// モデルの大きさ取得
	float GetXLength() { return (m_X_Max - m_X_Min); }
	float GetYLength() { return (m_Y_Max - m_Y_Min); }
	float GetZLength() { return (m_Z_Max - m_Z_Min); }

	// 頂点を取得
	std::vector<Vertex>	GetOriginVertex();
	// インデックスを取得
	std::vector<Index>	GetOriginIndex();

	// 回転、スケール変換した後の頂点を取得
	std::vector<Vertex> GetVertex(DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);

	// 解放処理
	HRESULT Release();
	// デストラクタ
	virtual ~FbxModel(){ Release(); }
};

#endif