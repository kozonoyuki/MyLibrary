//--------------------------------------------------------------------------------------
// SRVRenderer.h
// シェーダリソースビューをレンダリングするクラス
//--------------------------------------------------------------------------------------
#ifndef _SRV_RENDERER_
#define _SRV_RENDERER_

#include <string>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "DirectUtility.h"

class SRVRenderer
{
private:
	unsigned int m_IndexesCount;
	unsigned int m_VertexesCount;

	ID3D11VertexShader*			m_pVertexShader = nullptr;
	ID3D11PixelShader*			m_pPixelShader = nullptr;
	ID3D11InputLayout*			m_pVertexLayout = nullptr;
	ID3D11Buffer*				m_pVertexBuffer = nullptr;
	ID3D11Buffer*				m_pIndexBuffer = nullptr;
	ID3D11Buffer*				m_pConstantBuffer = nullptr;
	ID3D11ShaderResourceView*	m_pSRView = nullptr;
	ID3D11SamplerState*			m_pSamplerState = nullptr;

public:
	HRESULT LoadSRVRenderer(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, DirectX::XMFLOAT3 pos, float width, float height, float w_window, float h_window);
	HRESULT DrawSRView(ID3D11DeviceContext* pd3dDeviceContext, ID3D11ShaderResourceView* pSRView);
	HRESULT Release();
	~SRVRenderer();
};

#endif