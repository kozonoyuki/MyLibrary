//--------------------------------------------------------------------------------------
// FBXMesh.h
// ���f�����b�V���N���X�@���f�����̊e���b�V�������Ǘ�����
//--------------------------------------------------------------------------------------
#ifndef _COMMON_MESH_
#define _COMMON_MESH_

#include <d3d11_1.h>
#include <directxmath.h>
#include <vector>
#include <climits>
#include "FBXLoader.h"
#include "DirectUtility.h"

class FBXMesh
{
private:
	unsigned int m_IndexesCount;	// �C���f�b�N�X��
	unsigned int m_VertexesCount;	// ���_��

	ID3D11Buffer*				m_pVertexBuffer = nullptr;
	ID3D11Buffer*				m_pIndexBuffer = nullptr;
	ID3D11ShaderResourceView*	m_pTextureRV = nullptr;

public:
	std::vector<Vertex> m_Vertex;	// ���_
	std::vector<Index> m_Index;		// �C���f�b�N�X

	float m_X_Min;				// x���W�̍ŏ��l
	float m_X_Max;				// x���W�̍ő�l
	float m_Y_Min;				// y���W�̍ŏ��l
	float m_Y_Max;				// y���W�̍ő�l
	float m_Z_Min;				// z���W�̍ŏ��l
	float m_Z_Max;				// z���W�̍ő�l

	// �R���X�g���N�^
	FBXMesh();
	// ���b�V���f�[�^���o�b�t�@�Ɋi�[����
	HRESULT LoadMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, const ModelMesh & mesh, std::string texturename);
	// ���b�V���o�b�t�@���Z�b�g����
	HRESULT SetMesh(ID3D11DeviceContext* pd3dDeviceContext, bool shadowmap);
	// �f�X�g���N�^
	~FBXMesh();

};

#endif