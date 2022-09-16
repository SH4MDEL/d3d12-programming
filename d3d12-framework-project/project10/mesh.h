#pragma once
#include "stdafx.h"

struct Vertex
{
	Vertex(const XMFLOAT3& p, const XMFLOAT4& c) : position{ p }, color{ c } { }
	~Vertex() = default;
	XMFLOAT3 position;
	XMFLOAT4 color;
};

class Mesh
{
public:
	Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<Vertex>& vertices, const vector<UINT>& indices);
	~Mesh() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& m_commandList) const;
	void Render(const ComPtr<ID3D12GraphicsCommandList>& m_commandList, const D3D12_VERTEX_BUFFER_VIEW& instanceBufferView) const;
	void ReleaseUploadBuffer();

private:
	UINT						m_nVertices;
	ComPtr<ID3D12Resource>		m_vertexBuffer;
	ComPtr<ID3D12Resource>		m_vertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView;

	UINT						m_nIndices;
	ComPtr<ID3D12Resource>		m_indexBuffer;
	ComPtr<ID3D12Resource>		m_indexUploadBuffer;
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView;
};

class HeightMapImage
{
public:
	HeightMapImage(const wstring& fileName, INT width, INT length, XMFLOAT3 scale);
	~HeightMapImage() = default;

	FLOAT GetHeight(FLOAT x, FLOAT z) const;	// (x, z) 위치의 픽셀 값에 기반한 지형 높이 반환
	XMFLOAT3 GetNormal(INT x, INT z) const;		// (x, z) 위치의 법선 벡터 반환

	XMFLOAT3 GetScale() { return m_scale; }
	BYTE* GetPixels() { return m_pixels.get(); }
	INT GetWidth() { return m_width; }
	INT GetLength() { return m_length; }
private:
	unique_ptr<BYTE[]>			m_pixels;	// 높이 맵 이미지 픽셀(8-비트)들의 이차원 배열이다. 각 픽셀은 0~255의 값을 갖는다.
	INT							m_width;	// 높이 맵 이미지의 가로와 세로 크기이다
	INT							m_length;
	XMFLOAT3					m_scale;	// 높이 맵 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 스케일 벡터이다
};

class HeightMapGridMesh : public Mesh
{
public:
	HeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
		int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void* pContext = NULL);
	virtual ~HeightMapGridMesh();

	virtual void ReleaseUploadBuffers() {
		m_pd3dVertexUploadBuffer->Release(); m_pd3dIndexUploadBuffer->Release(); m_pd3dNormalUploadBuffer->Release();
	}

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	//격자의 좌표가 (x, z)일 때 교점(정점)의 높이를 반환하는 함수이다. 
	virtual float OnGetHeight(int x, int z, void* pContext);
	//격자의 좌표가 (x, z)일 때 교점(정점)의 색상을 반환하는 함수이다. 
	virtual XMFLOAT4 OnGetColor(int x, int z, void* pContext);
	virtual XMFLOAT3 OnGetAverageNormal(int x, int z, void* pContext);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);
protected:
	//격자의 크기(가로: x-방향, 세로: z-방향)이다. 
	int m_nWidth;
	int m_nLength;
	//격자의 스케일(가로: x-방향, 세로: z-방향, 높이: y-방향) 벡터이다. 
	//실제 격자 메쉬의 각 정점의 x-좌표, y-좌표, z-좌표는 스케일 벡터의 x-좌표, y-좌표, z-좌표로 곱한 값을 갖는다. 
	//즉, 실제 격자의 x-축 방향의 간격은 1이 아니라 스케일 벡터의 x-좌표가 된다. 
	//이렇게 하면 작은 격자(적은 정점)를 사용하더라도 큰 크기의 격자(지형)를 생성할 수 있다.
	XMFLOAT3 m_xmf3Scale;
};