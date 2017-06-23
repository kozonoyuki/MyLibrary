//--------------------------------------------------------------------------------------
// FBXMesh.h
// モデルメッシュクラス　モデル内の各メッシュ情報を管理する
//--------------------------------------------------------------------------------------
#ifndef _COMMON_MESH_
#define _COMMON_MESH_

#include <d3d11_1.h>
#include <directxmath.h>
#include <vector>
#include <climits>
#include "FBXLoader.h"
#include "DirectUtility.h"

class FBXMesh
{
private:
	unsigned int m_IndexesCount;	// インデックス数
	unsigned int m_VertexesCount;	// 頂点数

	ID3D11Buffer*				m_pVertexBuffer = nullptr;
	ID3D11Buffer*				m_pIndexBuffer = nullptr;
	ID3D11ShaderResourceView*	m_pTextureRV = nullptr;

public:
	std::vector<Vertex> m_Vertex;	// 頂点
	std::vector<Index> m_Index;		// インデックス

	float m_X_Min;				// x座標の最小値
	float m_X_Max;				// x座標の最大値
	float m_Y_Min;				// y座標の最小値
	float m_Y_Max;				// y座標の最大値
	float m_Z_Min;				// z座標の最小値
	float m_Z_Max;				// z座標の最大値

	// コンストラクタ
	FBXMesh();
	// メッシュデータをバッファに格納する
	HRESULT LoadMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, const ModelMesh & mesh, std::string texturename);
	// メッシュバッファをセットする
	HRESULT SetMesh(ID3D11DeviceContext* pd3dDeviceContext, bool shadowmap);
	// デストラクタ
	~FBXMesh();

};

#endif