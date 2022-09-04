#include "object.h"

GameObject::GameObject() : m_right{ 1.0f, 0.0f, 0.0f }, m_up{ 0.0f, 1.0f, 0.0f }, m_front{ 0.0f, 0.0f, 1.0f }, m_roll{ 0.0f }, m_pitch{ 0.0f }, m_yaw{ 0.0f }
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	m_mesh->ReleaseUploadBuffer();
}

void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	// 객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다.
	UpdateShaderVariable(commandList);

	if (m_mesh) m_mesh->Render(commandList);
}

void GameObject::Move(const XMFLOAT3& shift)
{
	SetPosition(Vector3::Add(GetPosition(), shift));
}

void GameObject::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
	// 회전
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(roll), XMConvertToRadians(pitch), XMConvertToRadians(yaw)) };
	XMMATRIX worldMatrix{ rotate * XMLoadFloat4x4(&m_worldMatrix) };
	XMStoreFloat4x4(&m_worldMatrix, worldMatrix);

	// 로컬 x,y,z축 최신화
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotate));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotate));
	XMStoreFloat3(&m_front, XMVector3TransformNormal(XMLoadFloat3(&m_front), rotate));
}

void GameObject::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	// PSO 설정
	if (m_shader) commandList->SetPipelineState(m_shader->GetPipelineState().Get());

	// 게임오브젝트의 월드 변환 행렬 최신화
	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	// 객체의 월드 변환 행렬을 루트 상수(32-비트 값)을 통하여 셰이더 변수(상수 버퍼)로 복사한다.
	commandList->SetGraphicsRoot32BitConstants(0, 16, &worldMatrix, 0);
}

void GameObject::SetMesh(const shared_ptr<Mesh>& mesh)
{
	if (m_mesh) m_mesh.reset();
	m_mesh = mesh;
}

void GameObject::SetShader(const shared_ptr<Shader>& shader)
{
	if (m_shader) m_shader.reset();
	m_shader = shader;
}

void GameObject::SetPosition(const XMFLOAT3& position)
{
	m_worldMatrix._41 = position.x;
	m_worldMatrix._42 = position.y;
	m_worldMatrix._43 = position.z;
}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3{ m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43 };
}

void GameObject::ReleaseUploadBuffer() const
{
	if (m_mesh) m_mesh->ReleaseUploadBuffer();
}


RotatingObject::RotatingObject() : m_rotationSpeed(100.0f)
{

}

void RotatingObject::Update(FLOAT timeElapsed)
{
	GameObject::Rotate(0.0f, m_rotationSpeed * timeElapsed, 0.0f);
}