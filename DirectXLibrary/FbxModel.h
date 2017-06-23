//--------------------------------------------------------------------------------------
// FbxModel.h
// FbxSdk��p�������f���N���X
//--------------------------------------------------------------------------------------

#ifndef _FBX_MODEL_
#define _FBX_MODEL_

#include "CommonModel.h"
#include "fbxsdk.h"
#include "FBXLoader.h"
#include "FBXMesh.h"

class FbxModel : public CommonModel{
private:

	FBXLoader* fbxloader = nullptr;

	std::vector<FBXMesh*> fbxmesh;

	typedef enum
	{
		eUnknown,
		eNull,
		eMarker,
		eSkeleton,
		eMesh,
		eNurbs,
		ePatch,
		eCamera,
		eCameraStereo,
		eCameraSwitcher,
		eLight,
		eOpticalReference,
		eOpticalMarker,
		eNurbsCurve,
		eTrimNurbsSurface,
		eBoundary,
		eNurbsSurface,
		eShape,
		eLODGroup,
		eSubDiv,
		eCachedEffect,
		eLine
	} AttribType;

	float light_z;

public:
	std::string m_filename;
	std::string m_texturename;
	std::string m_shadername;

public:
	FbxModel();
	// ���f���f�[�^���o�b�t�@�Ɋi�[����
	HRESULT LoadModel(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, std::string filename, std::string shadername);
	// ���f���f�[�^���V�F�[�_�ɑ��M����
	HRESULT DrawModel(ID3D11DeviceContext* pd3dDeviceContext, const DirectX::XMMATRIX & view, const DirectX::XMMATRIX & projection, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, bool shadowmap, DirectX::XMFLOAT4 eyepos);

	// ���f���̑傫���擾
	float GetXLength() { return (m_X_Max - m_X_Min); }
	float GetYLength() { return (m_Y_Max - m_Y_Min); }
	float GetZLength() { return (m_Z_Max - m_Z_Min); }

	// ���_���擾
	std::vector<Vertex>	GetOriginVertex();
	// �C���f�b�N�X���擾
	std::vector<Index>	GetOriginIndex();

	// ��]�A�X�P�[���ϊ�������̒��_���擾
	std::vector<Vertex> GetVertex(DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);

	// �������
	HRESULT Release();
	// �f�X�g���N�^
	virtual ~FbxModel(){ Release(); }
};

#endif