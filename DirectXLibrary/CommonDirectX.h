#pragma once

//--------------------------------------------------------------------------------------
// CommonDirectX.h
// DirectX�̃��b�p�[�N���X
//--------------------------------------------------------------------------------------
#include <Windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <string>
#include <directxcolors.h>
#include "DDSTextureLoader.h"
#include "FbxModel.h"
#include "Sprite.h"
#include "Line.h"
#include "SRVRenderer.h"

// refcount warning �o����������g�p����
//#define _COM_DEBUG_

class CommonDirectX {
private:
	HINSTANCE               m_hInst = nullptr;
	HWND                    m_hWnd = nullptr;
	D3D_DRIVER_TYPE			m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL		m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*			m_pd3dDevice = nullptr;
	ID3D11DeviceContext*	m_pd3dDeviceContext = nullptr;
	IDXGISwapChain*			m_pSwapChain = nullptr;
	ID3D11RenderTargetView*	m_pRenderTargetView = nullptr;
	ID3D11Texture2D*		m_pDepthStencil = nullptr;
	ID3D11DepthStencilView*	m_pDepthStencilView = nullptr;
	ID3D11ShaderResourceView*	m_pShaderResourceView = nullptr;
	ID3D11DepthStencilState*m_pDepthStencilState = nullptr;
	ID3D11BlendState*		m_pBlendState = nullptr;
	D3D11_VIEWPORT			m_ViewPort;

	// �V���h�E�}�b�v�p
	ID3D11Texture2D*			m_pShadowMap = nullptr;
	ID3D11DepthStencilView*		m_pShadowMapDepthStencilView = nullptr;
	ID3D11ShaderResourceView*	m_pShadowMapShaderResourceView = nullptr;
	D3D11_VIEWPORT				m_ViewPortShadowMap;

	// �V�F�[�_�[���\�[�X�r���[�`��p
	SRVRenderer*			m_pSRVRenderer = nullptr;

	struct registerModel
	{
		CommonModel* model;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 scale;
	};

	struct registerSprite
	{
		Sprite* sprite;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 scale;
	};

	struct registerLine
	{
		Line* line;
		DirectX::XMFLOAT3 startPos;
		DirectX::XMFLOAT3 endPos;
	};
	// 1�t���[���ŕ`�悳��郂�f���I�u�W�F�N�g��o�^
	std::vector<registerModel> m_registerModel;
	std::vector<registerSprite> m_registerSprite;
	std::vector<registerLine> m_registerLine;

	DirectX::XMMATRIX		m_View;			// ���_�ϊ��s��
	DirectX::XMMATRIX		m_Projection;	// ���e�ϊ��s��
	DirectX::XMFLOAT4		m_EyePos;

public:
	// DirectX�̏�����
	HRESULT Init(HINSTANCE hInstance, int nCmdShow, WNDPROC WndProc, UINT width, UINT height);
	// ���f���f�[�^�̓ǂݍ���
	CommonModel* LoadModel(std::string filename, std::string shadername);
	// ���f���f�[�^���V�F�[�_�ɑ��M
	HRESULT DrawModel(CommonModel* model, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);
	// �X�v���C�g�ǂݍ���
	Sprite* LoadSprite(float width, float height, std::string texturename, std::string shadername);
	// �X�v���C�g�`��
	HRESULT DrawSprite(Sprite* sprite, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);
	// ���ǂݍ���
	Line* LoadLine();
	// ���`��
	HRESULT DrawLine(Line* line, DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 endPos);
	// ���f���Ȃǂ�`�悷��O�̏���
	void DrawStart();
	// ���f���Ȃǂ�`�悵�I�������̏���
	void DrawEnd();
	// �J�����ʒu��ݒ�
	HRESULT SetCameraPosition(DirectX::XMVECTOR eye, DirectX::XMVECTOR at, DirectX::XMVECTOR up, float angle, float aspect, float nearz, float farz);

	// �u�����h�X�e�[�g�ݒ�̂��߂̃I�v�V����
	enum mBlendState { BLEND_DEFAULT, BLEND_ALIGNMENT, BLEND_ADD, BLEND_SUBTRACT, BLEND_MULTIPLE };
	HRESULT SetBlendState(mBlendState state);

	// �������
	HRESULT Release();
	// �f�X�g���N�^
	~CommonDirectX();

	SIZE GetWindowSize();

private:
	// �V���h�E�}�b�v�pCOM��������
	HRESULT InitShadowMap();
};