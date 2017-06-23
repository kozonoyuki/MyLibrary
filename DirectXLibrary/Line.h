//--------------------------------------------------------------------------------------
// Line.h
// 線描画クラス
//--------------------------------------------------------------------------------------

#ifndef _LINE_
#define _LINE_

#include <Windows.h>
#include <string> 
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "DirectUtility.h"

class Line {
protected:
	unsigned int IndexesCount;	// インデックスの数
	unsigned int VertexesCount;	// 頂点の数
	
	ID3D11VertexShader*			m_pVertexShader = nullptr;
	ID3D11PixelShader*			m_pPixelShader = nullptr;
	ID3D11InputLayout*			m_pVertexLayout = nullptr;
	ID3D11Buffer*				m_pVertexBuffer = nullptr;
	ID3D11Buffer*				m_pIndexBuffer = nullptr;
	ID3D11Buffer*				m_pConstantBuffer = nullptr;

	float m_ColorRed;
	float m_ColorGreen;
	float m_ColorBlue;
	float m_ColorAlpha;

public:
	// コンストラクタ
	Line();
	// 線の初期化
	HRESULT LoadLine(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext);
	// 線をシェーダに送信
	HRESULT DrawLine(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 endPos);
	// 線の色
	void SetLineColor(float r, float g, float b, float a);
	// 解放処理
	HRESULT Release();
	// デストラクタ
	~Line();
};

#endif
