#include "stdafx.h"
#include "SRVRenderer.h"

HRESULT SRVRenderer::LoadSRVRenderer(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, DirectX::XMFLOAT3 pos, float width, float height, float w_window, float h_window)
{
	HRESULT hr = S_OK;

	// 頂点シェーダをコンパイル
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(StringToWcstring("Model\\SRVShader.fx"), "VS", "vs_4_0", &pVSBlob);
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
	hr = CompileShaderFromFile(StringToWcstring("Model\\SRVShader.fx"), "PS", "ps_4_0", &pPSBlob);
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

	m_VertexesCount = 4;
	SimpleVertex box_vertex[4];
	box_vertex[0].Pos.x = 0.0f;
	box_vertex[0].Pos.y = height;
	box_vertex[0].Pos.z = 0.0f;
	box_vertex[0].Tex.x = 0.0f;
	box_vertex[0].Tex.y = 1.0f;

	box_vertex[1].Pos.x = width;
	box_vertex[1].Pos.y = height;
	box_vertex[1].Pos.z = 0.0f;
	box_vertex[1].Tex.x = 1.0f;
	box_vertex[1].Tex.y = 1.0f;

	box_vertex[2].Pos.x = width;
	box_vertex[2].Pos.y = 0.0f;
	box_vertex[2].Pos.z = 0.0f;
	box_vertex[2].Tex.x = 1.0f;
	box_vertex[2].Tex.y = 0.0f;

	box_vertex[3].Pos.x = 0.0f;
	box_vertex[3].Pos.y = 0.0f;
	box_vertex[3].Pos.z = 0.0f;
	box_vertex[3].Tex.x = 0.0f;
	box_vertex[3].Tex.y = 0.0f;

	m_IndexesCount = 6;
	WORD box_index[6] = { 0, 3, 1, 1, 3, 2 };

	// 頂点バッファを作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex)* m_VertexesCount;
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
	bd.ByteWidth = sizeof(WORD)* m_IndexesCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = box_index;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// コンスタントバッファを作成
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SRVRendererConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	// 2D描画用射影変換行列
	DirectX::XMMATRIX projection(
		2 / w_window, 0.0f, 0.0f, 0.0f,
		0.0f, -2 / h_window, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
		);

	// World行列を初期化
	DirectX::XMMATRIX World = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&pos));

	SRVRendererConstantBuffer cb;
	cb.mWorld = DirectX::XMMatrixTranspose(World);
	cb.mProjection = DirectX::XMMatrixTranspose(projection);

	// コンスタントバッファを登録
	pd3dDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);


	// サンプラを作成
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = 0.0f;
	sampDesc.BorderColor[1] = 1.0f;
	sampDesc.BorderColor[2] = 0.0f;
	sampDesc.BorderColor[3] = 0.0f;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerState);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT SRVRenderer::DrawSRView(ID3D11DeviceContext* pd3dDeviceContext, ID3D11ShaderResourceView* pSRView)
{
	HRESULT hr = S_OK;

	m_pSRView = const_cast<ID3D11ShaderResourceView*>(pSRView);

	pd3dDeviceContext->IASetInputLayout(m_pVertexLayout);							// インプットレイアウトをセット
	pd3dDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pd3dDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);						// ピクセルシェーダをセット
	pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pSRView);					// ピクセルシェーダにテクスチャをセット
	pd3dDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);						// ピクセルシェーダにサンプラをセット

	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	pd3dDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pd3dDeviceContext->DrawIndexed(m_IndexesCount, 0, 0);

	return hr;
}

HRESULT SRVRenderer::Release()
{
	HRESULT hr = S_OK;
	SAFE_RELEASE(m_pSamplerState);
	//SAFE_RELEASE(m_pSRView);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	return hr;
}

SRVRenderer::~SRVRenderer()
{
	this->Release();
}