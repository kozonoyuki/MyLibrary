//--------------------------------------------------------------------------------------
// CommonModel.h
// モデル基本クラス　各モデル拡張子クラスに派生する
//--------------------------------------------------------------------------------------
#ifndef _COMMON_MODEL_
#define _COMMON_MODEL_

#include <Windows.h>
#include <string>
#include <d3d11_1.h>
#include <directxmath.h>
#include <memory>
#include <vector>
#include "DirectUtility.h"

class CommonModel {
protected:

	unsigned int IndexesCount;	// インデックスの数
	unsigned int VertexesCount;	// 頂点の数

	std::vector<Vertex>		m_Vertex;	// 頂点
	std::vector<Index>		m_Index;	// インデックス

	float m_X_Min;				// x座標の最小値
	float m_X_Max;				// x座標の最大値
	float m_Y_Min;				// y座標の最小値
	float m_Y_Max;				// y座標の最大値
	float m_Z_Min;				// z座標の最小値
	float m_Z_Max;				// z座標の最大値

	ID3D11VertexShader*			m_pVertexShader = nullptr;
	ID3D11PixelShader*			m_pPixelShader = nullptr;
	ID3D11InputLayout*			m_pVertexLayout = nullptr;
	ID3D11Buffer*				m_pConstantBuffer = nullptr;
	ID3D11RasterizerState*		m_pRasterizerState = nullptr;
	ID3D11SamplerState*			m_pSamplerState[2];

	// シャドウマップ用
	ID3D11VertexShader*			m_pVertexShaderShadowMap = nullptr;
	ID3D11InputLayout*			m_pVertexLayoutShadowMap = nullptr;
	ID3D11RasterizerState*		m_pRasterizerStateShadowMap = nullptr;

public:
	// モデルデータをバッファに格納する
	virtual HRESULT LoadModel(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, std::string filename, std::string shadername) = 0;
	// モデルデータをシェーダに送信
	virtual HRESULT DrawModel(ID3D11DeviceContext* pd3dDeviceContext, const DirectX::XMMATRIX & view, const DirectX::XMMATRIX & projection, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, bool shadowmap, DirectX::XMFLOAT4 eyepos) = 0;

	// モデルの大きさ取得
	virtual float GetXLength() = 0;
	virtual float GetYLength() = 0;
	virtual float GetZLength() = 0;

	// 頂点を取得
	virtual std::vector<Vertex>	GetOriginVertex() = 0;
	virtual std::vector<Index>	GetOriginIndex() = 0;
	virtual std::vector<Vertex>	GetVertex(DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale) = 0;

	// 解放処理
	virtual HRESULT Release() = 0;
	// デストラクタ
	virtual ~CommonModel() = 0 {};
};

#endif