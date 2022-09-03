#pragma once
#include "stdafx.h"
#include "mesh.h"
#include "shader.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Update() { };
	virtual void Move(const XMFLOAT3& shift);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);
	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void SetMesh(const shared_ptr<Mesh>& Mesh);
	void SetShader(const shared_ptr<Shader>& shader);

	void SetPosition(const XMFLOAT3& position);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetRight() const { return m_right; }
	XMFLOAT3 GetUp() const { return m_up; }
	XMFLOAT3 GetFront() const { return m_front; }

	void ReleaseUploadBuffer() const;

protected:
	XMFLOAT4X4			m_worldMatrix;	// 월드 변환

	XMFLOAT3			m_right;		// 로컬 x축
	XMFLOAT3			m_up;			// 로컬 y축
	XMFLOAT3			m_front;		// 로컬 z축

	FLOAT				m_roll;			// x축 회전각
	FLOAT				m_pitch;		// y축 회전각
	FLOAT				m_yaw;			// z축 회전각

	shared_ptr<Mesh>	m_mesh;			// 메쉬
	shared_ptr<Shader>	m_shader;		// 셰이더
};