#ifndef _DIRECT_UTILITY_
#define _DIRECT_UTILITY_

#include <d3dcompiler.h>
#include <tchar.h>
#include "DDSTextureLoader.h"

// インタフェースのメモリ解放
#define SAFE_RELEASE(x) if (x != nullptr) { int cnt = x->Release(); x = nullptr; }

// D3DCompileを利用してシェーダをコンパイルするヘルパー関数
// プログラム実行時のシェーダコンパイルについて、
// デバッグでは実行可能だが、windows7にはd3dcompiler_47.dllがないため実行できない
static HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}



// std::stringがからw_char*型に変換する
static wchar_t* StringToWcstring(std::string str) {
	wchar_t * wcstr = new wchar_t[str.size() + 1];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstr, str.size() + 1, str.c_str(), _TRUNCATE);
	return wcstr;
}

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
};

struct Index
{
	WORD Idx[3];
};

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 Tex;
};

struct NormalVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 Tex;
	DirectX::XMFLOAT3 Norm;
};

struct ColorVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Col;
};

struct CommonVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 Tex;
	DirectX::XMFLOAT4 Col;
};

//__declspec(align(16))
struct ConstantBuffer
{
public:
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

struct ConvertConstantBuffer
{
public:
	DirectX::XMMATRIX mScale;
	DirectX::XMMATRIX mRotation;
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	DirectX::XMMATRIX mSMWVP;
	DirectX::XMFLOAT4 mDiffuse;
	DirectX::XMFLOAT4 mEyePos;
	DirectX::XMFLOAT4 mFogColor;
	DirectX::XMFLOAT3 mLight;
	float mNear;
	float mFar;
	float mParamOne;
	float mParamTwo;
};

struct SpriteConstantBuffer
{
public:
	DirectX::XMMATRIX mProjection;
	DirectX::XMMATRIX mScale;
	DirectX::XMMATRIX mRotation;
	DirectX::XMMATRIX mWorld;
	float mUvLeft;
	float mUvTop;
	float mUvWidth;
	float mUvHeight;
	float mUvAlpha;
	float mUvPivot_x;
	float mUvPivot_y;
};

struct LineConstantBuffer
{
public:
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

struct SRVRendererConstantBuffer
{
	DirectX::XMMATRIX mProjection;
	DirectX::XMMATRIX mWorld;
};

#endif