//--------------------------------------------------------------------------------------
// Splite.h
// 2D描画クラス
//--------------------------------------------------------------------------------------
#ifndef _SPLITE_
#define _SPLITE_

#include <Windows.h>
#include <string>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "DirectUtility.h"

class Sprite {
protected:
	unsigned int IndexesCount;	// インデックスの数
	unsigned int VertexesCount;	// 頂点の数

	// UV切り取り用変数
	float m_uv_left;	// 左上座標のx軸
	float m_uv_top;		// 左上座標のy軸
	float m_uv_width;	// 横幅
	float m_uv_height;	// 縦幅
	float m_uv_alpha;	// アルファ値
	float m_uv_pivot_x;	// 回転用ピボットx軸
	float m_uv_pivot_y;	// 回転用ピボットy軸

	ID3D11VertexShader*			m_pVertexShader = nullptr;
	ID3D11PixelShader*			m_pPixelShader = nullptr;
	ID3D11InputLayout*			m_pVertexLayout = nullptr;
	ID3D11Buffer*				m_pVertexBuffer = nullptr;
	ID3D11Buffer*				m_pIndexBuffer = nullptr;
	ID3D11Buffer*				m_pConstantBuffer = nullptr;
	ID3D11ShaderResourceView*	m_pTextureRV = nullptr;
	ID3D11SamplerState*			m_pSamplerLinear = nullptr;

public:
	// スプライトをバッファに格納する
	HRESULT LoadSprite(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, float width, float height, std::string texturename, std::string shadername);
	// テクスチャの切り取り
	void SetUVClip(float left, float top, float width, float height);
	// アルファ値をセット
	void SetAlpha(float alpha);
	// 回転用ピボット値をセット
	void SetRotationPivot(float x, float y);
	// スプライトをシェーダに送信
	HRESULT DrawSprite(ID3D11DeviceContext* pd3dDeviceContext, float w_window, float h_window, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);
	// 解放処理
	HRESULT Release();
	// デストラクタ
	~Sprite();
};

#endif