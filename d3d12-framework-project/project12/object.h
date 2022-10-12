#pragma once
#include "stdafx.h"
#include "mesh.h"
#include "texture.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Update(FLOAT timeElapsed) { };
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Move(const XMFLOAT3& shift);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);

	void SetMesh(const Mesh& mesh);
	virtual void SetTexture(const shared_ptr<Texture>& texture);

	void SetPosition(const XMFLOAT3& position);

	XMFLOAT4X4 GetWorldMatrix() const { return m_worldMatrix; }
	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetRight() const { return m_right; }
	XMFLOAT3 GetUp() const { return m_up; }
	XMFLOAT3 GetFront() const { return m_front; }

	void ReleaseUploadBuffer() const;

protected:
	XMFLOAT4X4				m_worldMatrix;	// 월드 변환

	XMFLOAT3				m_right;		// 로컬 x축
	XMFLOAT3				m_up;			// 로컬 y축
	XMFLOAT3				m_front;		// 로컬 z축

	FLOAT					m_roll;			// x축 회전각
	FLOAT					m_pitch;		// y축 회전각
	FLOAT					m_yaw;			// z축 회전각

	unique_ptr<Mesh>		m_mesh;			// 메쉬
	shared_ptr<Texture>		m_texture;		// 텍스처
};

class HierarchyObject : public GameObject
{
public:
	HierarchyObject();
	~HierarchyObject() = default;

	virtual void SetTexture(const shared_ptr<Texture>& texture);

	shared_ptr<HierarchyObject> LoadGeometry(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName);
	shared_ptr<HierarchyObject> LoadFrameHierarchy(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, ifstream& in);

	void SetChild(const shared_ptr<HierarchyObject>& child);

private:
	string						m_frameName;	// 현재 프레임의 이름
	shared_ptr<HierarchyObject> m_parent;
	shared_ptr<HierarchyObject>	m_sibling;
	shared_ptr<HierarchyObject>	m_child;
};

class RotatingObject : public GameObject
{
public:
	RotatingObject();
	~RotatingObject() = default;

	virtual void Update(FLOAT timeElapsed);

	void SetRotationSpeed(FLOAT rotationSpeed);

private:
	FLOAT				m_rotationSpeed;
};

class HeightMapTerrain : public GameObject
{
public:
	HeightMapTerrain(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, 
		const wstring& fileName, INT width, INT length, INT blockWidth, INT blockLength, XMFLOAT3 scale);
	~HeightMapTerrain() = default;

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>&commandList) const;
	virtual void Move(const XMFLOAT3 & shift);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);

	void SetPosition(const XMFLOAT3 & position);

	XMFLOAT3 GetPosition() const { return m_blocks.front()->GetPosition(); }
	FLOAT GetHeight(FLOAT x, FLOAT z) const;
	XMFLOAT3 GetNormal(FLOAT x, FLOAT z) const;
	INT GetWidth() const { return m_width; }
	INT GetLength() const { return m_length; }
	XMFLOAT3 GetScale() const { return m_scale; }

private:
	unique_ptr<HeightMapImage>		m_heightMapImage;	// 높이맵 이미지
	vector<unique_ptr<GameObject>>	m_blocks;			// 블록들
	INT								m_width;			// 이미지의 가로 길이
	INT								m_length;			// 이미지의 세로 길이
	XMFLOAT3						m_scale;			// 확대 비율
};

class Skybox : public GameObject
{
public:
	Skybox(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
		FLOAT width, FLOAT length, FLOAT depth);
	~Skybox() = default;

	virtual void Update(FLOAT timeElapsed);
};