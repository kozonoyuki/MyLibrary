//--------------------------------------------------------------------------------------
// Line.h
// ���`��N���X
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
	unsigned int IndexesCount;	// �C���f�b�N�X�̐�
	unsigned int VertexesCount;	// ���_�̐�
	
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
	// �R���X�g���N�^
	Line();
	// ���̏�����
	HRESULT LoadLine(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext);
	// �����V�F�[�_�ɑ��M
	HRESULT DrawLine(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 endPos);
	// ���̐F
	void SetLineColor(float r, float g, float b, float a);
	// �������
	HRESULT Release();
	// �f�X�g���N�^
	~Line();
};

#endif
