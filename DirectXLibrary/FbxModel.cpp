//--------------------------------------------------------------------------------------
// FbxModel.cpp
// 
//--------------------------------------------------------------------------------------
#include "stdafx.h"
#include "FbxModel.h"

FbxModel::FbxModel()
{
	fbxloader = new FBXLoader();
	m_pSamplerState[0] = nullptr;
	m_pSamplerState[1] = nullptr;

	light_z = 13.0;
}
// 頂点を取得
std::vector<Vertex> FbxModel::GetOriginVertex() {
	return m_Vertex;
}
// インデックスを取得
std::vector<Index> FbxModel::GetOriginIndex() {
	return m_Index;
}

// 回転、スケール変換した後の頂点を取得 
std::vector<Vertex> FbxModel::GetVertex(DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	std::vector<Vertex> result;
	DirectX::XMMATRIX mScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX mRotation = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	for (Vertex v : m_Vertex)
	{
		DirectX::XMVECTOR vectorPos = DirectX::XMLoadFloat3(&v.Pos);
		vectorPos = DirectX::XMVector3Transform(vectorPos, mScale);
		vectorPos = DirectX::XMVector3Transform(vectorPos, mRotation);
		v.Pos = DirectX::XMFLOAT3(vectorPos.m128_f32[0], vectorPos.m128_f32[1], vectorPos.m128_f32[2]);
		result.push_back(v);
	}
	return result;
}

HRESULT FbxModel::LoadModel(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, std::string filename, std::string shadername) {

	m_filename = filename;
	m_shadername = shadername;

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

	// シャドウマップシェーダをコンパイル
	ID3DBlob* pVSSMBlob = nullptr;
	hr = CompileShaderFromFile(StringToWcstring(shadername), "VS_SM", "vs_4_0", &pVSSMBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// 頂点シェーダを作成
	hr = pd3dDevice->CreateVertexShader(pVSSMBlob->GetBufferPointer(), pVSSMBlob->GetBufferSize(), nullptr, &m_pVertexShaderShadowMap);
	if (FAILED(hr))
	{
		pVSSMBlob->Release();
		return hr;
	}

	// インプットレイアウトを定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// インプットレイアウトを作成
	hr = pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	hr = pd3dDevice->CreateInputLayout(layout, numElements, pVSSMBlob->GetBufferPointer(),
		pVSSMBlob->GetBufferSize(), &m_pVertexLayoutShadowMap);
	pVSSMBlob->Release();
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
	
 	fbxloader->Initialize(filename.c_str());

	int meshSize = fbxloader->GetMeshCount();	// メッシュの数
	for (int i = 0; i < meshSize; ++i)
	{ 
		auto mesh = fbxloader->GetMesh(i);				// i番目のメッシュを取得
		
		// 今のところディフューズテクスチャーのみ対応
		// ファイル名は .png のため、ファイルパスを Model\\Texture.dds になるよう変更 
		auto materialName = mesh.materialNameList[0];
		auto materialId = fbxloader->GetMaterialId(materialName);
		auto material = fbxloader->GetMaterial(materialId);
		auto diffuseTexture = material.diffuseTextureName;
		auto slash_pos = diffuseTexture.find_last_of('\\');
		diffuseTexture = diffuseTexture.substr(slash_pos+1, diffuseTexture.length());
		auto dot_pos = diffuseTexture.find_last_of('.');
		diffuseTexture = diffuseTexture.substr(0, dot_pos);
		diffuseTexture = diffuseTexture.append(".dds");
		diffuseTexture = std::string("Model\\").append(diffuseTexture);
		m_texturename = diffuseTexture;
		FBXMesh * tempfbxmesh = new FBXMesh();
		tempfbxmesh->LoadMesh(pd3dDevice, pd3dDeviceContext, mesh, diffuseTexture);
		this->fbxmesh.push_back(tempfbxmesh);
	}

	// ユーザ用にメッシュをまとめた頂点インデックスを作成
	WORD OffsetIndex = 0;
	for (int i = 0; i < meshSize; ++i)
	{
		for (auto j : this->fbxmesh[i]->m_Vertex)
		{
			this->m_Vertex.push_back(j);
		}
		for (auto j : fbxmesh[i]->m_Index)
		{
			j.Idx[0] += OffsetIndex;
			j.Idx[1] += OffsetIndex;
			j.Idx[2] += OffsetIndex;
			this->m_Index.push_back(j);
		}
		OffsetIndex += this->fbxmesh[i]->m_Vertex.size();
	}

	// ユーザ用にモデルの範囲を定義
	this->m_X_Max = FLT_MIN;
	this->m_X_Min = FLT_MAX;
	this->m_Y_Max = FLT_MIN;
	this->m_Y_Min = FLT_MAX;
	this->m_Z_Max = FLT_MIN;
	this->m_Z_Min = FLT_MAX;
	for (int i = 0; i < meshSize; ++i)
	{
		if (this->m_X_Max < this->fbxmesh[i]->m_X_Max) this->m_X_Max = this->fbxmesh[i]->m_X_Max;
		if (this->m_X_Min > this->fbxmesh[i]->m_X_Min) this->m_X_Min = this->fbxmesh[i]->m_X_Min;
		if (this->m_Y_Max < this->fbxmesh[i]->m_Y_Max) this->m_Y_Max = this->fbxmesh[i]->m_Y_Max;
		if (this->m_Y_Min > this->fbxmesh[i]->m_Y_Min) this->m_Y_Min = this->fbxmesh[i]->m_Y_Min;
		if (this->m_Z_Max < this->fbxmesh[i]->m_Z_Max) this->m_Z_Max = this->fbxmesh[i]->m_Z_Max;
		if (this->m_Z_Min > this->fbxmesh[i]->m_Z_Min) this->m_Z_Min = this->fbxmesh[i]->m_Z_Min;
	}

	// コンスタントバッファを作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConvertConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	// ラスタライザステートを設定
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0;
	rsDesc.SlopeScaledDepthBias = 0;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.MultisampleEnable = FALSE;
	rsDesc.AntialiasedLineEnable = FALSE;
	hr = pd3dDevice->CreateRasterizerState(&rsDesc, &m_pRasterizerState);
	if (FAILED(hr))
		return hr;

	// シャドウマップ用ラスタライザステートを設定
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0;
	rsDesc.SlopeScaledDepthBias = 0;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.MultisampleEnable = FALSE;
	rsDesc.AntialiasedLineEnable = FALSE;
	hr = pd3dDevice->CreateRasterizerState(&rsDesc, &m_pRasterizerStateShadowMap);
	if (FAILED(hr))
		return hr;

	// サンプラの作成
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 2;
	sampDesc.BorderColor[0] = 0.0f;
	sampDesc.BorderColor[1] = 0.0f;
	sampDesc.BorderColor[2] = 0.0f;
	sampDesc.BorderColor[3] = 0.0f;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerState[0]);
	if (FAILED(hr))
		return hr;

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.BorderColor[0] = 1.0f;
	sampDesc.BorderColor[1] = 1.0f;
	sampDesc.BorderColor[2] = 1.0f;
	sampDesc.BorderColor[3] = 1.0f;
	hr = pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerState[1]);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT FbxModel::DrawModel(ID3D11DeviceContext* pd3dDeviceContext, const DirectX::XMMATRIX & view, const DirectX::XMMATRIX & projection, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, bool shadowmap, DirectX::XMFLOAT4 eyepos) {
	HRESULT hr = S_OK;

	// World行列を初期化
	DirectX::XMMATRIX mWorld = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&pos));
	// scaleベクトルを行列に変換
	DirectX::XMMATRIX mScale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 角度(rotation)から行列に変換 ラジアン単位 = 度*XM_PI / 180
	DirectX::XMMATRIX mRotation = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	DirectX::XMMATRIX tWorld(mWorld);
	DirectX::XMMATRIX tView(view);
	DirectX::XMMATRIX tProjection(projection);
	/////////////////テスト/////////////////////////
	DirectX::XMFLOAT3 m_LightPos(0.0f, 30.0f, -15.0f);
	DirectX::XMFLOAT4 diffusecolor(1.0f, 1.0f, 1.0f, 1.0f);

	// 光源から見たベクトル
	DirectX::XMVECTOR focusPosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR upDirection = { 0.0f, 1.0f, 0.0f };
	DirectX::XMMATRIX shadowMapView = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_LightPos), focusPosition, upDirection);

 	DirectX::XMMATRIX shadowMapProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, 0.01f, 100.0f);

	DirectX::XMMATRIX SMWVP = tWorld * mScale * mRotation * shadowMapView * shadowMapProjection;

	ConvertConstantBuffer cb;

	if (shadowmap) 
	{

		cb.mScale = DirectX::XMMatrixTranspose(mScale);
		cb.mRotation = DirectX::XMMatrixTranspose(mRotation);
		cb.mWorld = DirectX::XMMatrixTranspose(tWorld);
		cb.mView = DirectX::XMMatrixTranspose(shadowMapView);
		cb.mProjection = DirectX::XMMatrixTranspose(shadowMapProjection);
		cb.mSMWVP = DirectX::XMMatrixTranspose(SMWVP);
		cb.mDiffuse = diffusecolor;
		cb.mLight = m_LightPos;
		cb.mEyePos = eyepos;
		cb.mFogColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		cb.mNear = 450.0f;
		cb.mFar = 500.0f;
		cb.mParamOne = 1.0f;
		cb.mParamTwo = 5.0f;

		int meshSize = fbxmesh.size();
		for (int i = 0; i < meshSize; ++i)
		{
			pd3dDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);
			pd3dDeviceContext->IASetInputLayout(m_pVertexLayoutShadowMap);
			pd3dDeviceContext->RSSetState(m_pRasterizerStateShadowMap);
			pd3dDeviceContext->VSSetShader(m_pVertexShaderShadowMap, nullptr, 0);
			pd3dDeviceContext->PSSetShader(nullptr, nullptr, 0);
			pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
			//pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
			//pd3dDeviceContext->PSSetSamplers(0, 2, m_pSamplerState);

			pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			fbxmesh[i]->SetMesh(pd3dDeviceContext, shadowmap);
		}
	}
	else
	{

		DirectX::XMVECTOR vec = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&m_LightPos), tView);

		cb.mScale = DirectX::XMMatrixTranspose(mScale);
		cb.mRotation = DirectX::XMMatrixTranspose(mRotation);
		cb.mWorld = DirectX::XMMatrixTranspose(tWorld);
		cb.mView = DirectX::XMMatrixTranspose(tView);
		cb.mProjection = DirectX::XMMatrixTranspose(tProjection);
		cb.mSMWVP = DirectX::XMMatrixTranspose(SMWVP);
		cb.mDiffuse = diffusecolor;
		DirectX::XMStoreFloat3(&cb.mLight, vec);
		cb.mEyePos = eyepos;
		cb.mFogColor = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		// ToDO フォグ値などを設定するメソッドを作成
		cb.mNear = 450.0f;
		cb.mFar = 500.0f;
		cb.mParamOne = 1.0f;
		cb.mParamTwo = 5.0f;

		int meshSize = fbxmesh.size();
		for (int i = 0; i < meshSize; ++i)
		{
			pd3dDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);	// コンスタントバッファを更新
			pd3dDeviceContext->IASetInputLayout(m_pVertexLayout);							// インプットレイアウトをセット
			pd3dDeviceContext->RSSetState(m_pRasterizerState);
			pd3dDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);					// 頂点シェーダをセット
			pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);				// 頂点シェーダにコンスタントバッファをセット
			pd3dDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);						// ピクセルシェーダをセット
			pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);				// ピクセルシェーダにコンスタントバッファをセット
			//pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTextureRV);	
			pd3dDeviceContext->PSSetSamplers(0, 2, m_pSamplerState);						// ピクセルシェーダにサンプラをセット

			pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// メッシュ描画の最後にこの命令を行う
			fbxmesh[i]->SetMesh(pd3dDeviceContext, shadowmap);

			/*
			UINT stride = sizeof(SimpleVertex);
			UINT offset = 0;
			pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

			pd3dDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
			*/
			//pd3dDeviceContext->DrawIndexed(IndexesCount, 0, 0);
		}
	}
	
	return hr;
}
 
HRESULT FbxModel::Release() {
	HRESULT hr = S_OK;
	if (fbxloader != nullptr)
	{
		fbxloader->Finalize();
		delete fbxloader;
		fbxloader = nullptr;
	}
	for (int i = 0; i < fbxmesh.size(); ++i)
	{
		if (fbxmesh[i] != nullptr)
		{
			delete fbxmesh[i];
			fbxmesh[i] = nullptr;
		}
	}

	SAFE_RELEASE(m_pSamplerState[0])
	SAFE_RELEASE(m_pSamplerState[1]);
	SAFE_RELEASE(m_pRasterizerStateShadowMap);
	SAFE_RELEASE(m_pVertexLayoutShadowMap);
	SAFE_RELEASE(m_pRasterizerState);
	//SAFE_RELEASE(m_pTextureRV);
	//SAFE_RELEASE(m_pVertexBuffer);
	//SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShaderShadowMap);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	
	return hr;
}
