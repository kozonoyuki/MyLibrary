//--------------------------------------------------------------------------------------
// Line.cpp
// ���`��N���X
//--------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Line.h"

Line::Line() {
	IndexesCount = 2;
	VertexesCount = 2;
	m_ColorRed = 1.0f;
	m_ColorGreen = 1.0f;
	m_ColorBlue = 1.0f;
	m_ColorAlpha = 1.0f;
}

// ���̏�����
HRESULT Line::LoadLine(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext) {
	HRESULT hr = S_OK;
	// ���_�V�F�[�_���R���p�C��
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(_T("Model\\LineShader.fx"), "VS", "vs_4_0", &pVSBlob);
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
	hr = CompileShaderFromFile(_T("Model\\LineShader.fx"), "PS", "ps_4_0", &pPSBlob);
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

	// �C���f�b�N�X�o�b�t�@�Ɋi�[
	WORD box_index[2] = { 0, 1 };

	// �C���f�b�N�X�o�b�t�@���쐬
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD)*IndexesCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = box_index;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// �R���X�^���g�o�b�t�@���쐬
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(LineConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	return hr;
}
// �����V�F�[�_�ɑ��M
HRESULT Line::DrawLine(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 endPos) {
	HRESULT hr = S_OK;
	// ���_���o�b�t�@�Ɋi�[
	ColorVertex box_vertex[8];
	box_vertex[0].Pos.x = startPos.x;
	box_vertex[0].Pos.y = startPos.y;
	box_vertex[0].Pos.z = startPos.z;
	box_vertex[0].Col.x = m_ColorRed;
	box_vertex[0].Col.y = m_ColorGreen;
	box_vertex[0].Col.z = m_ColorBlue;
	box_vertex[0].Col.w = m_ColorAlpha;
	box_vertex[1].Pos.x = endPos.x;
	box_vertex[1].Pos.y = endPos.y;
	box_vertex[1].Pos.z = endPos.z;
	box_vertex[1].Col.x = m_ColorRed;
	box_vertex[1].Col.y = m_ColorGreen;
	box_vertex[1].Col.z = m_ColorBlue;
	box_vertex[1].Col.w = m_ColorAlpha;

	// ���_�o�b�t�@���쐬 ���_�o�b�t�@�͕`��x�ɍ쐬�A�C���f�b�N�X�o�b�t�@�̓C���X�^���X�쐬�̂Ƃ��̂ݍ쐬
	SAFE_RELEASE(m_pVertexBuffer);
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ColorVertex)* VertexesCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = box_vertex;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// �R���X�^���g�o�b�t�@���쐬
	LineConstantBuffer cb;
	DirectX::XMMATRIX tView(view);
	DirectX::XMMATRIX tProjection(projection);
	cb.mView = DirectX::XMMatrixTranspose(tView);
	cb.mProjection = DirectX::XMMatrixTranspose(tProjection);

	pd3dDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	pd3dDeviceContext->IASetInputLayout(m_pVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pd3dDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	UINT stride = sizeof(ColorVertex);
	UINT offset = 0;

	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pd3dDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pd3dDeviceContext->DrawIndexed(IndexesCount, 0, 0);

	return hr;
}
// ���̐F
void Line::SetLineColor(float r, float g, float b, float a) {
	this->m_ColorRed = r;
	this->m_ColorGreen = g;
	this->m_ColorBlue = b;
	this->m_ColorAlpha = a;
}
// �������
HRESULT Line::Release() {
	HRESULT hr = S_OK;
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	return hr;
}
// �f�X�g���N�^
Line::~Line() {
	this->Release();
}
