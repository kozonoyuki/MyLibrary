//--------------------------------------------------------------------------------------
// CommonDirectX.cpp
// 
//--------------------------------------------------------------------------------------
#include "stdafx.h"
#include "CommonDirectX.h"

// シャドウマップ用COMを初期化
HRESULT CommonDirectX::InitShadowMap()
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = 512;
	descDepth.Height = 512;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &m_pShadowMap);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateDepthStencilView(m_pShadowMap, &descDSV, &m_pShadowMapDepthStencilView);
	if (FAILED(hr))
		return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;
	hr = m_pd3dDevice->CreateShaderResourceView(m_pShadowMap, &srDesc,&m_pShadowMapShaderResourceView);
	if (FAILED(hr))
		return hr;

	m_ViewPortShadowMap.TopLeftX = 0.0f;
	m_ViewPortShadowMap.TopLeftY = 0.0f;
	m_ViewPortShadowMap.Width = 512.0f;
	m_ViewPortShadowMap.Height = 512.0f;
	m_ViewPortShadowMap.MinDepth = 0.0f;
	m_ViewPortShadowMap.MaxDepth = 1.0f;

	return hr;
}

// 初期化
HRESULT CommonDirectX::Init(HINSTANCE hInstance, int nCmdShow, WNDPROC WndProc, UINT width, UINT height)
{
	HRESULT hr = S_OK;

	// ウインドウ
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = nullptr;
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	m_hInst = hInstance;
	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	m_hWnd = CreateWindow(L"TutorialWindowClass", L"Template DirectX11", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!m_hWnd)
		return E_FAIL;

	ShowWindow(m_hWnd, nCmdShow);

	// デバイスの作成
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,	// ハードウェアドライバ
		D3D_DRIVER_TYPE_WARP,		// ソフトウェア・ラスタライザ
		D3D_DRIVER_TYPE_REFERENCE,	// リファレンスラスタライザ
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	// スワップチェインの設定
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pd3dDeviceContext);

		if (hr == E_INVALIDARG)
		{
			hr = D3D11CreateDeviceAndSwapChain(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_featureLevel, &m_pd3dDeviceContext);
		}

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	//
	
#ifdef _DEBUG
#ifdef _COM_DEBUG_
	ID3D11Debug *d3dDebug = nullptr;
	hr = m_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr)) {
		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
		d3dDebug->Release();
		if (FAILED(hr))
			return hr;
	} else {
		d3dDebug->Release();
		return hr;
	}
#endif
#endif
	
	// レンダーターゲットビューの作成
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;
	
	// 深度ステンシルテクスチャの作成
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	//descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencil);
	if (FAILED(hr))
		return hr;
		
	// 深度ステンシルビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	//descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = -1;
	hr = m_pd3dDevice->CreateShaderResourceView(m_pDepthStencil, &srDesc, &m_pShaderResourceView);
	if (FAILED(hr))
		return hr;

	// 描画ターゲットと深度ステンシルビューを設定
	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	
	// 深度ステンシルステートの設定
	D3D11_DEPTH_STENCIL_DESC descDSS;
	ZeroMemory(&descDSS, sizeof(descDSS));
	descDSS.DepthEnable = TRUE;
	descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDSS.DepthFunc = D3D11_COMPARISON_LESS;
	descDSS.StencilEnable = FALSE;
	descDSS.StencilReadMask = 0xff;
	descDSS.StencilWriteMask = 0xff;
	descDSS.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	descDSS.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	descDSS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	descDSS.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	descDSS.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	descDSS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	descDSS.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = m_pd3dDevice->CreateDepthStencilState(&descDSS, &m_pDepthStencilState);
	if (FAILED(hr))
		return hr;

	// 深度ステンシルオブジェクトを設定
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
	
	/*
	// ブレンドステートオブジェクトの設定
	D3D11_BLEND_DESC descBlend;
	ZeroMemory(&descBlend, sizeof(descBlend));
	descBlend.AlphaToCoverageEnable = FALSE;
	descBlend.IndependentBlendEnable = FALSE;

	descBlend.RenderTarget[0].BlendEnable = TRUE;
	descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = m_pd3dDevice->CreateBlendState(&descBlend, &m_pBlendState);
	if (FAILED(hr))
		return hr;
	*/
	this->SetBlendState(BLEND_ALIGNMENT);

	// ビューポートのセットアップ
	//D3D11_VIEWPORT vp;
	m_ViewPort.Width = (FLOAT)width;
	m_ViewPort.Height = (FLOAT)height;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_pd3dDeviceContext->RSSetViewports(1, &m_ViewPort);

	hr = InitShadowMap();
	if (FAILED(hr))
		return hr;

	m_pSRVRenderer = new SRVRenderer();
	m_pSRVRenderer->LoadSRVRenderer(m_pd3dDevice, m_pd3dDeviceContext, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 200, 200, width, height);

	return S_OK;
}

HRESULT CommonDirectX::SetCameraPosition(DirectX::XMVECTOR eye, DirectX::XMVECTOR at, DirectX::XMVECTOR up, float angle, float aspect, float nearz, float farz)
{
	HRESULT hr = S_OK;

	// View行列を設定
	m_View = DirectX::XMMatrixLookAtLH(eye, at, up);
	DirectX::XMStoreFloat4(&m_EyePos, eye);

	// Projection行列を設定
	m_Projection = DirectX::XMMatrixPerspectiveFovLH(angle, aspect, nearz, farz);

	return hr;
}

HRESULT CommonDirectX::Release()
{
	
	HRESULT hr = S_OK;

	if (m_pSRVRenderer != nullptr) delete m_pSRVRenderer;

	m_registerModel.clear();
	m_registerSprite.clear();
	m_registerLine.clear();

	if (m_pd3dDeviceContext) {
		m_pd3dDeviceContext->ClearState();
	}
	/*
	SAFE_RELEASE(m_pShadowMap);
	SAFE_RELEASE(m_pShadowMapDepthStencilView);
	SAFE_RELEASE(m_pShadowMapShaderResourceView);
	SAFE_RELEASE(m_pBlendState);
	SAFE_RELEASE(m_pShaderResourceView);
	SAFE_RELEASE(m_pDepthStencilState);
	SAFE_RELEASE(m_pDepthStencil);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pd3dDeviceContext);
	SAFE_RELEASE(m_pd3dDevice);
	*/
	// refcountが不正なら表示するコード
	// IUnknownのAddRefを行っていないためいらないかもしれない
	ULONG cnt;
	TCHAR s[256];
	
	
	if (m_pShadowMap) {
		cnt = m_pShadowMap->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX ShadowMapの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pShadowMap = nullptr;
	}
	if (m_pShadowMapDepthStencilView) {
		cnt = m_pShadowMapDepthStencilView->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX ShadowMapDepthStencilViewの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pShadowMapDepthStencilView = nullptr;
	}
	if (m_pShadowMapShaderResourceView) {
		cnt = m_pShadowMapShaderResourceView->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX ShadowMapShaderResourceViewの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pShadowMapShaderResourceView = nullptr;
	}
	if (m_pBlendState) {
		cnt = m_pBlendState->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX BlendStateの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pBlendState = nullptr;
	}
	if (m_pShaderResourceView) {
		cnt = m_pShaderResourceView->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX ShaderResourceViewの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pShaderResourceView = nullptr;
	}
	if (m_pDepthStencilState) {
		cnt = m_pDepthStencilState->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX DepthStencilStateの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pDepthStencilState = nullptr;
	}
	if (m_pDepthStencil) {
		cnt = m_pDepthStencil->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX DepthStencilの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pDepthStencil = nullptr;
	}
	if (m_pDepthStencilView) {
		cnt = m_pDepthStencilView->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX DepthStencilViewの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pDepthStencilView = nullptr;
	}
	if (m_pRenderTargetView) {
		cnt = m_pRenderTargetView->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX RenderTargetViewの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pRenderTargetView = nullptr;
	}
	if (m_pSwapChain) {
		cnt = m_pSwapChain->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX SwapChainの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pSwapChain = nullptr;
	}
	if (m_pd3dDeviceContext) {
		cnt = m_pd3dDeviceContext->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX DeviceContextの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pd3dDeviceContext = nullptr;
	}
	
	if (m_pd3dDevice) {
		cnt = m_pd3dDevice->Release();
		if (cnt != 0) {
			_stprintf_s(s, _T("★ CommonDirectX Deviceの参照カウンタが0ではありません。REF : %d\n"), cnt);
			OutputDebugString(s);
		}
		m_pd3dDevice = nullptr;
	}
	
	return hr;
}

void CommonDirectX::DrawStart()
{
	// 登録モデルを初期化
	m_registerModel.clear();
	m_registerSprite.clear();
	m_registerLine.clear();

	// バックバッファをクリア
	m_pd3dDeviceContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::Black);
	m_pd3dDeviceContext->ClearDepthStencilView(m_pShadowMapDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_pd3dDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CommonDirectX::DrawEnd()
{
	// デバイスコンテキストと、視点、投影行列、位置、回転、倍率を引数にし、モデルデータをシェーダに渡す
	// ブレンドステートオブジェクトを設定
	this->SetBlendState(BLEND_ALIGNMENT);
	m_pd3dDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 0);

	// パス1 シャドウマップ
	//m_pd3dDeviceContext->ClearState();
	m_pd3dDeviceContext->RSSetViewports(1, &m_ViewPortShadowMap);
	ID3D11RenderTargetView* nullRender[1] = { nullptr };
	m_pd3dDeviceContext->OMSetRenderTargets(1, nullRender, m_pShadowMapDepthStencilView);
	
	for (registerModel model : m_registerModel)
	{
		model.model->DrawModel(m_pd3dDeviceContext, m_View, m_Projection, model.pos, model.rotation, model.scale, true, m_EyePos);
	}

	// パス2 最終出力
	//m_pd3dDeviceContext->ClearState();
	m_pd3dDeviceContext->RSSetViewports(1, &m_ViewPort);

	    // 描画ターゲットと深度ステンシルビューを設定
	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// シャドウマップ描画
	m_pSRVRenderer->DrawSRView(m_pd3dDeviceContext, m_pShadowMapShaderResourceView);

	m_pd3dDeviceContext->PSSetShaderResources(1, 1, &m_pShadowMapShaderResourceView);
	//m_pd3dDeviceContext->OMSetBlendState(m_pBlendState, BlendFactor, 0xffffffff);
	for (registerModel model : m_registerModel)
	{
		model.model->DrawModel(m_pd3dDeviceContext, m_View, m_Projection, model.pos, model.rotation, model.scale, false, m_EyePos);
	}

	this->SetBlendState(BLEND_ALIGNMENT);
	RECT rec;
	GetWindowRect(m_hWnd, &rec);
	for (registerSprite sprite : m_registerSprite)
	{
		sprite.sprite->DrawSprite(m_pd3dDeviceContext, (float)rec.right - (float)rec.left, (float)rec.bottom - (float)rec.top, sprite.pos, sprite.rotation, sprite.scale);
	}

	for (registerLine line : m_registerLine)
	{
		line.line->DrawLine(m_pd3dDevice, m_pd3dDeviceContext, m_View, m_Projection, line.startPos, line.endPos);
	}
	
	// バックバッファをフロントバッファにする
	m_pSwapChain->Present(0, 0);

	ID3D11ShaderResourceView* pnull[] = { nullptr, nullptr };
	m_pd3dDeviceContext->PSSetShaderResources(0, 2, pnull);
}

CommonModel* CommonDirectX::LoadModel(std::string filename, std::string shadername)
{
	// モデルクラスを作成、デバイス、デバイスコンテキストを引数にロードする
	// モデルの拡張子によってFbxModelクラス、**クラスなど判断
	FbxModel* model = new FbxModel();
	model->LoadModel(m_pd3dDevice, m_pd3dDeviceContext, filename, shadername);
	return model;
}

HRESULT CommonDirectX::DrawModel(CommonModel* model, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	// 描画モデルを登録
	registerModel registerModel;
	registerModel.model = model;
	registerModel.pos = pos;
	registerModel.rotation = rotation;
	registerModel.scale = scale;
	m_registerModel.push_back(registerModel);
	
	return S_OK;
}

Sprite* CommonDirectX::LoadSprite(float width, float height, std::string texturename, std::string shadername)
{
	Sprite* sprite = new Sprite();
	sprite->LoadSprite(m_pd3dDevice, m_pd3dDeviceContext, width, height, texturename, shadername);
	return sprite;
}

HRESULT CommonDirectX::DrawSprite(Sprite* sprite, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	registerSprite registerSprite;
	registerSprite.sprite = sprite;
	registerSprite.pos = pos;
	registerSprite.rotation = rotation;
	registerSprite.scale = scale;
	m_registerSprite.push_back(registerSprite);
	/*
	RECT rec;
	GetWindowRect(m_hWnd, &rec);
	// ブレンドステートオブジェクトを設定
	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pd3dDeviceContext->OMSetBlendState(m_pBlendState, BlendFactor, 0xffffffff);
	splite->DrawSprite(m_pd3dDeviceContext, (float)rec.right - (float)rec.left, (float)rec.bottom - (float)rec.top, pos, rotation, scale);
	*/
	return S_OK;
}

// 線読み込み
Line* CommonDirectX::LoadLine()
{
	Line* line = new Line();
	line->LoadLine(m_pd3dDevice, m_pd3dDeviceContext);
	return line;
}
// 線描画
HRESULT CommonDirectX::DrawLine(Line* line, DirectX::XMFLOAT3 startPos, DirectX::XMFLOAT3 endPos)
{
	registerLine registerLine;
	registerLine.line = line;
	registerLine.startPos = startPos;
	registerLine.endPos = endPos;
	m_registerLine.push_back(registerLine);
	return S_OK;
}

HRESULT CommonDirectX::SetBlendState(mBlendState state) 
{
	HRESULT hr = S_OK;

	D3D11_BLEND_DESC descBlend;
	ZeroMemory(&descBlend, sizeof(descBlend));
	descBlend.AlphaToCoverageEnable = FALSE;
	descBlend.IndependentBlendEnable = FALSE;

	switch (state) {
	case BLEND_DEFAULT:	// ブレンドステート無効
		descBlend.RenderTarget[0].BlendEnable = FALSE;
		descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_ALIGNMENT:// 線形合成用ブレンド
		descBlend.RenderTarget[0].BlendEnable = TRUE;
		descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_ADD:		// 加算合成用ブレンド
		descBlend.RenderTarget[0].BlendEnable = TRUE;
		descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_SUBTRACT:	// 減算合成用ブレンド
		descBlend.RenderTarget[0].BlendEnable = TRUE;
		descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MULTIPLE:	// 積算合成用ブレンド
		descBlend.RenderTarget[0].BlendEnable = TRUE;
		descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	default:		// ブレンドステート無効
		descBlend.RenderTarget[0].BlendEnable = FALSE;
		descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	}

	SAFE_RELEASE(m_pBlendState);

	hr = m_pd3dDevice->CreateBlendState(&descBlend, &m_pBlendState);
	if (FAILED(hr))
		return hr;

	// ブレンドステートオブジェクトを設定
	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pd3dDeviceContext->OMSetBlendState(m_pBlendState, BlendFactor, 0xffffffff);

	return hr;
}

CommonDirectX::~CommonDirectX()
{
	this->Release();
}
//ウィンドウの幅を取得
SIZE CommonDirectX::GetWindowSize()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	SIZE size = { rect.right - rect.left, rect.bottom - rect.top };
	return size;
}