//--------------------------------------------------------------------------------------
// CommonModel.h
// ���f����{�N���X�@�e���f���g���q�N���X�ɔh������
//--------------------------------------------------------------------------------------
#ifndef _COMMON_MODEL_
#define _COMMON_MODEL_

#include <Windows.h>
#include <string>
#include <d3d11_1.h>
#include <directxmath.h>
#include <memory>
#include <vector>
#include "DirectUtility.h"

class CommonModel {
protected:

	unsigned int IndexesCount;	// �C���f�b�N�X�̐�
	unsigned int VertexesCount;	// ���_�̐�

	std::vector<Vertex>		m_Vertex;	// ���_
	std::vector<Index>		m_Index;	// �C���f�b�N�X

	float m_X_Min;				// x���W�̍ŏ��l
	float m_X_Max;				// x���W�̍ő�l
	float m_Y_Min;				// y���W�̍ŏ��l
	float m_Y_Max;				// y���W�̍ő�l
	float m_Z_Min;				// z���W�̍ŏ��l
	float m_Z_Max;				// z���W�̍ő�l

	ID3D11VertexShader*			m_pVertexShader = nullptr;
	ID3D11PixelShader*			m_pPixelShader = nullptr;
	ID3D11InputLayout*			m_pVertexLayout = nullptr;
	ID3D11Buffer*				m_pConstantBuffer = nullptr;
	ID3D11RasterizerState*		m_pRasterizerState = nullptr;
	ID3D11SamplerState*			m_pSamplerState[2];

	// �V���h�E�}�b�v�p
	ID3D11VertexShader*			m_pVertexShaderShadowMap = nullptr;
	ID3D11InputLayout*			m_pVertexLayoutShadowMap = nullptr;
	ID3D11RasterizerState*		m_pRasterizerStateShadowMap = nullptr;

public:
	// ���f���f�[�^���o�b�t�@�Ɋi�[����
	virtual HRESULT LoadModel(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, std::string filename, std::string shadername) = 0;
	// ���f���f�[�^���V�F�[�_�ɑ��M
	virtual HRESULT DrawModel(ID3D11DeviceContext* pd3dDeviceContext, const DirectX::XMMATRIX & view, const DirectX::XMMATRIX & projection, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, bool shadowmap, DirectX::XMFLOAT4 eyepos) = 0;

	// ���f���̑傫���擾
	virtual float GetXLength() = 0;
	virtual float GetYLength() = 0;
	virtual float GetZLength() = 0;

	// ���_���擾
	virtual std::vector<Vertex>	GetOriginVertex() = 0;
	virtual std::vector<Index>	GetOriginIndex() = 0;
	virtual std::vector<Vertex>	GetVertex(DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale) = 0;

	// �������
	virtual HRESULT Release() = 0;
	// �f�X�g���N�^
	virtual ~CommonModel() = 0 {};
};

#endif