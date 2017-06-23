#pragma once

//--------------------------------------------------------------------------------------
// CommonDirectX.h
// DirectXのラッパークラス
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

// refcount warning 出現時これを使用する
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

	// シャドウマップ用
	ID3D11Texture2D*			m_pShadowMap = nullptr;
	ID3D11DepthStencilView*		m_pShadowMapDepthStencilView = nullptr;
	ID3D11ShaderResourceView*	m_pShadowMapShaderResourceView = nullptr;
	D3D11_VIEWPORT				m_ViewPortShadowMap;

	// シェーダーリソースビュー描画用
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
	// 1フレームで描画されるモデルオブジェクトを登録
	std::vector<registerModel> m_registerModel;
	std::vector<registerSprite> m_registerSprite;
	std::vector<registerLine> m_registerLine;

	DirectX::XMMATRIX		m_View;			// 視点変換行列
	DirectX::XMMATRIX		m_Projection;	// 投影変換行列
	DirectX::XMFLOAT4		m_EyePos;

public:
	// DirectXの初期化
	HRESULT Init(HINSTANCE hInstance, int nCmdShow, WNDPROC WndProc, UINT width, UINT height);
	// モデルデータの読み込み
	CommonModel* LoadModel(std::string filename, std::string shadername);
	// モデルデータをシェーダに送信
	HRESULT DrawModel(CommonModel* model, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);
	// スプライト読み込み
	Sprite* LoadSprite(float width, float height, std::string texturename, std::string shadername);
	// スプライト描画
	HRESULT DrawSprite(Sprite* sprite, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);
	// 線読み込み
	Line* LoadLine();
	// 線描画
	HRESULT DrawLine(Line* line, DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 endPos);
	// モデルなどを描画する前の処理
	void DrawStart();
	// モデルなどを描画し終わった後の処理
	void DrawEnd();
	// カメラ位置を設定
	HRESULT SetCameraPosition(DirectX::XMVECTOR eye, DirectX::XMVECTOR at, DirectX::XMVECTOR up, float angle, float aspect, float nearz, float farz);

	// ブレンドステート設定のためのオプション
	enum mBlendState { BLEND_DEFAULT, BLEND_ALIGNMENT, BLEND_ADD, BLEND_SUBTRACT, BLEND_MULTIPLE };
	HRESULT SetBlendState(mBlendState state);

	// 解放処理
	HRESULT Release();
	// デストラクタ
	~CommonDirectX();

	SIZE GetWindowSize();

private:
	// シャドウマップ用COMを初期化
	HRESULT InitShadowMap();
};