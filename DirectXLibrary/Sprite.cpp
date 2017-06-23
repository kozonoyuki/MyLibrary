//--------------------------------------------------------------------------------------
// Sprite.cpp
// 
//--------------------------------------------------------------------------------------
#include "stdafx.h"
#include "Sprite.h"

HRESULT Sprite::LoadSprite(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, float width, float height, std::string texturename, std::string shadername) {
	HRESULT hr = S_OK;
	// 頂点シェーダをコンパイル
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(StringToWcstring(shadername), "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// 頂点シェーダを作成
	hr = pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// インプットレイアウトを定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// インプットレイアウトを作成
	hr = pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// ピクセルシェーダをコンパイル
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(StringToWcstring(shadername), "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// ピクセルシェーダを作成
	hr = pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// 頂点バッファの配列数
	VertexesCount = 4;

	//UV切り取り用初期値
	m_uv_left = 0.0f;
	m_uv_top = 0.0f;
	m_uv_width = 1.0f;
	m_uv_height = 1.0f;

	//テクスチャのアルファ値
	m_uv_alpha = 1.0f;

	// ピボット値
	m_uv_pivot_x = 0.0f;
	m_uv_pivot_y = 0.0f;

	// ここの場合、頂点の順番は 左上、右上、右下、左下とする。
	CommonVertex box_vertex[4];
	box_vertex[0].Pos.x = 0.0f;
	box_vertex[0].Pos.y = height;
	box_vertex[0].Pos.z = 0.0f;
	box_vertex[0].Tex.x = 0.0f;
	box_vertex[0].Tex.y = 1.0f;
	box_vertex[0].Col.x = 1.0f;
	box_vertex[0].Col.y = 1.0f;
	box_vertex[0].Col.z = 1.0f;
	box_vertex[0].Col.w = 1.0f;

	box_vertex[1].Pos.x = width;
	box_vertex[1].Pos.y = height;
	box_vertex[1].Pos.z = 0.0f;
	box_vertex[1].Tex.x = 1.0f;
	box_vertex[1].Tex.y = 1.0f;
	box_vertex[1].Col.x = 1.0f;
	box_vertex[1].Col.y = 1.0f;
	box_vertex[1].Col.z = 1.0f;
	box_vertex[1].Col.w = 1.0f;

	box_vertex[2].Pos.x = width;
	box_vertex[2].Pos.y = 0.0f;
	box_vertex[2].Pos.z = 0.0f;
	box_vertex[2].Tex.x = 1.0f;
	box_vertex[2].Tex.y = 0.0f;
	box_vertex[2].Col.x = 1.0f;
	box_vertex[2].Col.y = 1.0f;
	box_vertex[2].Col.z = 1.0f;
	box_vertex[2].Col.w = 1.0f;

	box_vertex[3].Pos.x = 0.0f;
	box_vertex[3].Pos.y = 0.0f;
	box_vertex[3].Pos.z = 0.0f;
	box_vertex[3].Tex.x = 0.0f;
	box_vertex[3].Tex.y = 0.0f;
	box_vertex[3].Col.x = 1.0f;
	box_vertex[3].Col.y = 1.0f;
	box_vertex[3].Col.z = 1.0f;
	box_vertex[3].Col.w = 1.0f;

	// インデックスは射影変換行列によりポリゴンの描画を右回りではなく左回りにしなければならない
	IndexesCount = 6;
	WORD box_index[6] = { 0, 3, 1, 1, 3, 2 };

	// 頂点バッファを作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CommonVertex)* VertexesCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = box_vertex;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// インデックスバッファを作成
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD)* IndexesCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = box_index;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// コンスタントバッファを作成
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SpriteConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
		return hr;
	
	// テクスチャの読み込み
	hr = DirectX::CreateDDSTextureFromFile(pd3dDevice, StringToWcstring(texturename), nullptr, &m_pTextureRV);
	if (FAILED(hr))
		return hr;

	// サンプラを作成
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = 1.0f;
	sampDesc.BorderColor[1] = 0.0f;
	sampDesc.BorderColor[2] = 0.0f;
	sampDesc.BorderColor[3] = 0.0f;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
	if (FAILED(hr))
		return hr;

	return hr;
}

void Sprite::SetUVClip(float left, float top, float width, float height) {
	if (0.0f <= left && 1.0f >= left && 0.0f <= top && 1.0f >= top && left + width <= 1.0f && top + height <= 1.0f) {
		m_uv_left = left; m_uv_top = top; m_uv_width = width; m_uv_height = height;
	}
}

void Sprite::SetAlpha(float alpha) {
	if (0.0f <= alpha && 1.0f >= alpha)
		m_uv_alpha = alpha;
}

void Sprite::SetRotationPivot(float x, float y) {
	m_uv_pivot_x = x;
	m_uv_pivot_y = y;
}

HRESULT Sprite::DrawSprite(ID3D11DeviceContext* pd3dDeviceContext, float w_window, float h_window, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale) {
	HRESULT hr = S_OK;

	// World行列を初期化
	DirectX::XMMATRIX World = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&pos));
	// scaleベクトルを行列に変換
	DirectX::XMMATRIX Scale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 角度(rotation)から行列に変換 ラジアン単位 = 度*XM_PI / 180
	DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	// 2D描画用射影変換行列
	DirectX::XMMATRIX projection(
									2/w_window,	0.0f,		0.0f,	0.0f,
									0.0f,		-2/h_window,0.0f,	0.0f,
									0.0f,		0.0f,		1.0f,	0.0f,
									-1.0f,		1.0f,		0.0f,	1.0f	
								);

	// コンスタントバッファの設定
	SpriteConstantBuffer cb;
	cb.mProjection = DirectX::XMMatrixTranspose(projection);
	cb.mScale = DirectX::XMMatrixTranspose(Scale);
	cb.mRotation = DirectX::XMMatrixTranspose(Rotation);
	cb.mWorld = DirectX::XMMatrixTranspose(World);
	cb.mUvLeft = m_uv_left;
	cb.mUvTop = m_uv_top;
	cb.mUvWidth = m_uv_width;
	cb.mUvHeight = m_uv_height;
	cb.mUvAlpha = m_uv_alpha;
	cb.mUvPivot_x = m_uv_pivot_x;
	cb.mUvPivot_y = m_uv_pivot_y;

	pd3dDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);	// コンスタントバッファを更新
	pd3dDeviceContext->IASetInputLayout(m_pVertexLayout);							// インプットレイアウトをセット
	pd3dDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);					// 頂点シェーダをセット
	pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);				// 頂点シェーダにコンスタントバッファをセット
	pd3dDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);						// ピクセルシェーダをセット
	pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);				// ピクセルシェーダにコンスタントバッファをセット
	pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTextureRV);					// ピクセルシェーダにテクスチャをセット
	pd3dDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);						// ピクセルシェーダにサンプラをセット

	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(CommonVertex);
	UINT offset = 0;
	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	pd3dDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pd3dDeviceContext->DrawIndexed(IndexesCount, 0, 0);
	return hr;
}

HRESULT Sprite::Release() {
	HRESULT hr = S_OK;
	SAFE_RELEASE(m_pSamplerLinear);
	SAFE_RELEASE(m_pTextureRV);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	/*
	ULONG cnt;
	TCHAR s[256];
	if (m_pSamplerLinear) {
		cnt = m_pSamplerLinear->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ Sprite SamplerLinerの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pSamplerLinear = nullptr;
	}
	if (m_pTextureRV) {
		cnt = m_pTextureRV->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ Sprite TextureRVの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pTextureRV = nullptr;
	}
	if (m_pVertexBuffer) {
		cnt = m_pVertexBuffer->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ Sprite VertexBufferの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pVertexBuffer = nullptr;
	}
	if (m_pIndexBuffer) {
		cnt = m_pIndexBuffer->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ Sprite IndexBufferの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pIndexBuffer = nullptr;
	}
	if (m_pConstantBuffer) {
		cnt = m_pConstantBuffer->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ Sprite ConstantBufferの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pConstantBuffer = nullptr;
	}
	if (m_pVertexLayout) {
		cnt = m_pVertexLayout->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ Sprite VertexLayoutの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pVertexLayout = nullptr;
	}
	if (m_pVertexShader) {
		cnt = m_pVertexShader->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ Sprite VertexShaderの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pVertexShader = nullptr;
	}
	if (m_pPixelShader) {
		cnt = m_pPixelShader->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ Sprite PixelShaderの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pPixelShader = nullptr;
	}
	*/
	return hr;
}

Sprite::~Sprite() {
	Release();
}