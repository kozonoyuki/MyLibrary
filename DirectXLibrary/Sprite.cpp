//--------------------------------------------------------------------------------------
// Sprite.cpp
// 
//--------------------------------------------------------------------------------------
#include "stdafx.h"
#include "Sprite.h"

HRESULT Sprite::LoadSprite(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, float width, float height, std::string texturename, std::string shadername) {
	HRESULT hr = S_OK;
	// ���_�V�F�[�_���R���p�C��
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(StringToWcstring(shadername), "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// ���_�V�F�[�_���쐬
	hr = pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// �C���v�b�g���C�A�E�g���`
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// �C���v�b�g���C�A�E�g���쐬
	hr = pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// �s�N�Z���V�F�[�_���R���p�C��
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(StringToWcstring(shadername), "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// �s�N�Z���V�F�[�_���쐬
	hr = pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// ���_�o�b�t�@�̔z��
	VertexesCount = 4;

	//UV�؂���p�����l
	m_uv_left = 0.0f;
	m_uv_top = 0.0f;
	m_uv_width = 1.0f;
	m_uv_height = 1.0f;

	//�e�N�X�`���̃A���t�@�l
	m_uv_alpha = 1.0f;

	// �s�{�b�g�l
	m_uv_pivot_x = 0.0f;
	m_uv_pivot_y = 0.0f;

	// �����̏ꍇ�A���_�̏��Ԃ� ����A�E��A�E���A�����Ƃ���B
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

	// �C���f�b�N�X�͎ˉe�ϊ��s��ɂ��|���S���̕`����E���ł͂Ȃ������ɂ��Ȃ���΂Ȃ�Ȃ�
	IndexesCount = 6;
	WORD box_index[6] = { 0, 3, 1, 1, 3, 2 };

	// ���_�o�b�t�@���쐬
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

	// �C���f�b�N�X�o�b�t�@���쐬
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD)* IndexesCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = box_index;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// �R���X�^���g�o�b�t�@���쐬
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SpriteConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
		return hr;
	
	// �e�N�X�`���̓ǂݍ���
	hr = DirectX::CreateDDSTextureFromFile(pd3dDevice, StringToWcstring(texturename), nullptr, &m_pTextureRV);
	if (FAILED(hr))
		return hr;

	// �T���v�����쐬
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

	// World�s���������
	DirectX::XMMATRIX World = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&pos));
	// scale�x�N�g�����s��ɕϊ�
	DirectX::XMMATRIX Scale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// �p�x(rotation)����s��ɕϊ� ���W�A���P�� = �x*XM_PI / 180
	DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	// 2D�`��p�ˉe�ϊ��s��
	DirectX::XMMATRIX projection(
									2/w_window,	0.0f,		0.0f,	0.0f,
									0.0f,		-2/h_window,0.0f,	0.0f,
									0.0f,		0.0f,		1.0f,	0.0f,
									-1.0f,		1.0f,		0.0f,	1.0f	
								);

	// �R���X�^���g�o�b�t�@�̐ݒ�
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

	pd3dDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);	// �R���X�^���g�o�b�t�@���X�V
	pd3dDeviceContext->IASetInputLayout(m_pVertexLayout);							// �C���v�b�g���C�A�E�g���Z�b�g
	pd3dDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);					// ���_�V�F�[�_���Z�b�g
	pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);				// ���_�V�F�[�_�ɃR���X�^���g�o�b�t�@���Z�b�g
	pd3dDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);						// �s�N�Z���V�F�[�_���Z�b�g
	pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);				// �s�N�Z���V�F�[�_�ɃR���X�^���g�o�b�t�@���Z�b�g
	pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTextureRV);					// �s�N�Z���V�F�[�_�Ƀe�N�X�`�����Z�b�g
	pd3dDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);						// �s�N�Z���V�F�[�_�ɃT���v�����Z�b�g

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
			_stprintf_s(s, _T("�� Sprite SamplerLiner�̎Q�ƃJ�E���^��0�ł͂���܂���BREF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pSamplerLinear = nullptr;
	}
	if (m_pTextureRV) {
		cnt = m_pTextureRV->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("�� Sprite TextureRV�̎Q�ƃJ�E���^��0�ł͂���܂���BREF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pTextureRV = nullptr;
	}
	if (m_pVertexBuffer) {
		cnt = m_pVertexBuffer->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("�� Sprite VertexBuffer�̎Q�ƃJ�E���^��0�ł͂���܂���BREF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pVertexBuffer = nullptr;
	}
	if (m_pIndexBuffer) {
		cnt = m_pIndexBuffer->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("�� Sprite IndexBuffer�̎Q�ƃJ�E���^��0�ł͂���܂���BREF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pIndexBuffer = nullptr;
	}
	if (m_pConstantBuffer) {
		cnt = m_pConstantBuffer->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("�� Sprite ConstantBuffer�̎Q�ƃJ�E���^��0�ł͂���܂���BREF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pConstantBuffer = nullptr;
	}
	if (m_pVertexLayout) {
		cnt = m_pVertexLayout->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("�� Sprite VertexLayout�̎Q�ƃJ�E���^��0�ł͂���܂���BREF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pVertexLayout = nullptr;
	}
	if (m_pVertexShader) {
		cnt = m_pVertexShader->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("�� Sprite VertexShader�̎Q�ƃJ�E���^��0�ł͂���܂���BREF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pVertexShader = nullptr;
	}
	if (m_pPixelShader) {
		cnt = m_pPixelShader->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("�� Sprite PixelShader�̎Q�ƃJ�E���^��0�ł͂���܂���BREF : %d\n"), cnt);
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