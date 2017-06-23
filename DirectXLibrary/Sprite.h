//--------------------------------------------------------------------------------------
// Splite.h
// 2D�`��N���X
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
	unsigned int IndexesCount;	// �C���f�b�N�X�̐�
	unsigned int VertexesCount;	// ���_�̐�

	// UV�؂���p�ϐ�
	float m_uv_left;	// ������W��x��
	float m_uv_top;		// ������W��y��
	float m_uv_width;	// ����
	float m_uv_height;	// �c��
	float m_uv_alpha;	// �A���t�@�l
	float m_uv_pivot_x;	// ��]�p�s�{�b�gx��
	float m_uv_pivot_y;	// ��]�p�s�{�b�gy��

	ID3D11VertexShader*			m_pVertexShader = nullptr;
	ID3D11PixelShader*			m_pPixelShader = nullptr;
	ID3D11InputLayout*			m_pVertexLayout = nullptr;
	ID3D11Buffer*				m_pVertexBuffer = nullptr;
	ID3D11Buffer*				m_pIndexBuffer = nullptr;
	ID3D11Buffer*				m_pConstantBuffer = nullptr;
	ID3D11ShaderResourceView*	m_pTextureRV = nullptr;
	ID3D11SamplerState*			m_pSamplerLinear = nullptr;

public:
	// �X�v���C�g���o�b�t�@�Ɋi�[����
	HRESULT LoadSprite(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, float width, float height, std::string texturename, std::string shadername);
	// �e�N�X�`���̐؂���
	void SetUVClip(float left, float top, float width, float height);
	// �A���t�@�l���Z�b�g
	void SetAlpha(float alpha);
	// ��]�p�s�{�b�g�l���Z�b�g
	void SetRotationPivot(float x, float y);
	// �X�v���C�g���V�F�[�_�ɑ��M
	HRESULT DrawSprite(ID3D11DeviceContext* pd3dDeviceContext, float w_window, float h_window, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);
	// �������
	HRESULT Release();
	// �f�X�g���N�^
	~Sprite();
};

#endif