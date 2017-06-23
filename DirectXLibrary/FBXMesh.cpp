#include "stdafx.h"
#include "FBXMesh.h"

FBXMesh::FBXMesh()
{
	this->m_X_Max = FLT_MIN;
	this->m_X_Min = FLT_MAX;
	this->m_Y_Max = FLT_MIN;
	this->m_Y_Min = FLT_MAX;
	this->m_Z_Max = FLT_MIN;
	this->m_Z_Min = FLT_MAX;
}

HRESULT FBXMesh::LoadMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, const ModelMesh & mesh, std::string texturename)
{
	HRESULT hr = S_OK;

	// 頂点、インデックス分バッファに送るデータを動的確保
	this->m_VertexesCount = mesh.vertexList.size();
	this->m_IndexesCount = mesh.indexList.size();
	NormalVertex* mesh_vertex = new NormalVertex[this->m_VertexesCount];
	WORD* mesh_index = new WORD[this->m_IndexesCount];
	for (int i = 0; i < this->m_VertexesCount; ++i)
	{
		//DirectX::XMStoreFloat3(&mesh_vertex[i].Pos, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mesh.vertexList[i].position), mesh.invMeshBaseposeMatrix));
		mesh_vertex[i].Pos = mesh.vertexList[i].position;
		mesh_vertex[i].Norm = mesh.vertexList[i].normal;
		mesh_vertex[i].Tex = mesh.vertexList[i].uv[0];
		mesh_vertex[i].Tex.y = 1.0f - mesh_vertex[i].Tex.y;

		// メンバ用データ格納 
		Vertex ver;
		ver.Pos  = mesh_vertex[i].Pos;
		m_Vertex.push_back(ver);
		// 頂点範囲
		if (ver.Pos.x > m_X_Max) m_X_Max = ver.Pos.x; if (ver.Pos.x < m_X_Min) m_X_Min = ver.Pos.x;
		if (ver.Pos.y > m_Y_Max) m_Y_Max = ver.Pos.y; if (ver.Pos.y < m_Y_Min) m_Y_Min = ver.Pos.y;
		if (ver.Pos.z > m_Z_Max) m_Z_Max = ver.Pos.z; if (ver.Pos.z < m_Z_Min) m_Z_Min = ver.Pos.z;

	}
	int idx_flag = 0;
	Index idx;
	for (int i = 0; i < this->m_IndexesCount; ++i)
	{
		mesh_index[i] = mesh.indexList[i];
		// メンバ用データ格納
		++idx_flag;
		int num = i % 3;
		idx.Idx[num] = mesh_index[i];
		if (idx_flag == 3)
		{
			m_Index.push_back(idx);
			idx_flag = 0;
		}
	}

	// 頂点バッファを作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(NormalVertex)* m_VertexesCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mesh_vertex;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// インデックスバッファを作成
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD)* m_IndexesCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = mesh_index;
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// テクスチャの読み込み
	hr = DirectX::CreateDDSTextureFromFile(pd3dDevice, StringToWcstring(texturename), nullptr, &m_pTextureRV);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT FBXMesh::SetMesh(ID3D11DeviceContext* pd3dDeviceContext, bool shadowmap)
{
	HRESULT hr = S_OK;
	if (!shadowmap) pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTextureRV);
	UINT stride = sizeof(NormalVertex);
	UINT offset = 0;
	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pd3dDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pd3dDeviceContext->DrawIndexed(m_IndexesCount, 0, 0);
	return hr;
}
FBXMesh::~FBXMesh()
{
	SAFE_RELEASE(m_pTextureRV);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}