#include "object.h"

GameObject::GameObject() : m_right{ 1.0f, 0.0f, 0.0f }, m_up{ 0.0f, 1.0f, 0.0f }, m_front{ 0.0f, 0.0f, 1.0f }, m_roll{ 0.0f }, m_pitch{ 0.0f }, m_yaw{ 0.0f }
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	if (m_mesh) m_mesh->ReleaseUploadBuffer();
}

void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &worldMatrix, 0);

	if (m_texture) { m_texture->UpdateShaderVariable(commandList); }

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

void GameObject::SetMesh(const Mesh& mesh)
{
	if (m_mesh) m_mesh.reset();
	m_mesh = make_unique<Mesh>(mesh);
}

void GameObject::SetTexture(const shared_ptr<Texture>& texture)
{
	if (m_texture) m_texture.reset();
	m_texture = texture;
}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3{ m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43 };
}

void GameObject::SetPosition(const XMFLOAT3& position)
{
	m_worldMatrix._41 = position.x;
	m_worldMatrix._42 = position.y;
	m_worldMatrix._43 = position.z;
}

void GameObject::ReleaseUploadBuffer() const
{
	if (m_mesh) m_mesh->ReleaseUploadBuffer();
	if (m_texture) m_texture->ReleaseUploadBuffer();

}

HierarchyObject::HierarchyObject() : GameObject()
{

}

shared_ptr<HierarchyObject> HierarchyObject::LoadGeometry(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName)
{
	ifstream in{ fileName };
	if (!in) return nullptr;

	return LoadFrameHierarchy(device, commandList, in);
}

shared_ptr<HierarchyObject> HierarchyObject::LoadFrameHierarchy(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, ifstream& in)
{
	BYTE strLength;
	INT frame;

	shared_ptr<HierarchyObject> gameObject;

	while (1) {
		in.read((char*)(&strLength), sizeof(BYTE));
		string strToken(strLength, '\0');
		in.read((&strToken[0]), sizeof(char) * strLength);

		if (strToken == "<Frame>:") {
			gameObject = make_shared<HierarchyObject>();

			in.read((char*)(&frame), sizeof(INT));

			in.read((char*)(&strLength), sizeof(BYTE));
			gameObject->m_frameName.reserve(strLength);
			in.read((char*)(&gameObject->m_frameName), sizeof(char) * strLength);
		}
		else if (strToken == "<Transform>:") {
			XMFLOAT3 position, rotation, scale;
			XMFLOAT4 qrotation;

			in.read((char*)(&position), sizeof(FLOAT) * 3);
			in.read((char*)(&rotation), sizeof(FLOAT) * 3);
			in.read((char*)(&scale), sizeof(FLOAT) * 3);
			in.read((char*)(&qrotation), sizeof(FLOAT) * 4);
		}
		else if (strToken == "<TransformMatrix>:") {
			in.read((char*)(&gameObject->m_worldMatrix), sizeof(FLOAT) * 16);
		}
		else if (strToken == "<Mesh>:") {
			unique_ptr<MeshFromFile> mesh;
			mesh->LoadMesh(device, commandList, in);
			m_mesh = move(mesh);
		}
		else if (strToken == "<Materials>:") {
			LoadMaterial(device, commandList, in);
		}
		else if (strToken == "<Children>:") {
			INT childNum = 0;
			in.read((char*)(&childNum), sizeof(INT));
			if (childNum) {
				for (int i = 0; i < childNum; ++i) {
					gameObject->SetChild(HierarchyObject::LoadFrameHierarchy(device, commandList, in));
				}
			}
		}
		else if (strToken == "</Frame>:") {
			break;
		}
	}
	return gameObject;
}

void HierarchyObject::LoadMaterial(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, ifstream& in)
{
	BYTE strLength;

	while (1) {
		in.read((char*)(&strLength), sizeof(BYTE));
		string strToken(strLength, '\0');
		in.read((&strToken[0]), sizeof(char) * strLength);

		if (strToken == "<Material>:") {
			in.read((char*)(&strLength), sizeof(BYTE));
			strToken.resize(strLength, '\0');
			in.read((char*)(&strToken), sizeof(char) * strLength);
		}
		else if (strToken == "<AlbedoColor>:") {
			XMFLOAT4 dummy;
			in.read((char*)(&dummy), sizeof(XMFLOAT4));
		}
		else if (strToken == "<EmissiveColor>:") {
			XMFLOAT4 dummy;
			in.read((char*)(&dummy), sizeof(XMFLOAT4));
		}
		else if (strToken == "<SpecularColor>:") {
			XMFLOAT4 dummy;
			in.read((char*)(&dummy), sizeof(XMFLOAT4));
		}
		else if (strToken == "<Glossiness>:") {
			FLOAT dummy;
			in.read((char*)(&dummy), sizeof(FLOAT));
		}
		else if (strToken == "<Smoothness>:") {
			FLOAT dummy;
			in.read((char*)(&dummy), sizeof(FLOAT));
		}
		else if (strToken == "<Metallic>:") {
			FLOAT dummy;
			in.read((char*)(&dummy), sizeof(FLOAT));
		}
		else if (strToken == "<SpecularHighlight>:") {
			FLOAT dummy;
			in.read((char*)(&dummy), sizeof(FLOAT));
		}
		else if (strToken == "<GlossyReflection>:") {
			FLOAT dummy;
			in.read((char*)(&dummy), sizeof(FLOAT));
		}
		else if (strToken == "</Materials>:") {
			break;
		}
	}
}

void HierarchyObject::SetChild(const shared_ptr<HierarchyObject>& child)
{
	if (m_child) {
		if (child) child->m_sibling = m_child->m_sibling;
		m_child->m_sibling = child;
	}
	else {
		m_child = child;
	}
	if (child) {
		child->m_parent = (shared_ptr<HierarchyObject>)this;
	}
}

RotatingObject::RotatingObject() : m_rotationSpeed(100.0f)
{

}

void RotatingObject::Update(FLOAT timeElapsed)
{
	GameObject::Rotate(0.0f, m_rotationSpeed * timeElapsed, 0.0f);
}

void RotatingObject::SetRotationSpeed(FLOAT rotationSpeed)
{
	m_rotationSpeed = rotationSpeed;
}

HeightMapTerrain::HeightMapTerrain(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const wstring& fileName, INT width, INT length, INT blockWidth, INT blockLength, XMFLOAT3 scale)
	: m_width{ width }, m_length{ length }, m_scale{ scale }
{
	// 높이맵이미지 로딩
	m_heightMapImage = make_unique<HeightMapImage>(fileName, m_width, m_length, m_scale);

	// 가로, 세로 블록의 개수
	int widthBlockCount{ m_width / blockWidth };
	int lengthBlockCount{ m_length / blockLength };

	// 블록 생성
	for (int z = 0; z < lengthBlockCount; ++z)
		for (int x = 0; x < widthBlockCount; ++x)
		{
			int xStart{ x * (blockWidth - 1) };
			int zStart{ z * (blockLength - 1) };
			unique_ptr<GameObject> block{ make_unique<GameObject>() };
			block->SetMesh(HeightMapGridMesh(device, commandList, xStart, zStart, blockWidth, blockLength, m_scale,m_heightMapImage.get()));
			m_blocks.push_back(move(block));
		}
}

void HeightMapTerrain::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (m_texture) m_texture->UpdateShaderVariable(commandList);
	for (const auto& block : m_blocks)
		block->Render(commandList);
}

void HeightMapTerrain::Move(const XMFLOAT3& shift)
{
	for (auto& block : m_blocks)
		block->Move(shift);
}

void HeightMapTerrain::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
	for (auto& block : m_blocks)
		block->Rotate(roll, pitch, yaw);
}

void HeightMapTerrain::SetPosition(const XMFLOAT3& position)
{
	// 지형의 위치 설정은 모든 블록들의 위치를 조정한다는 것임
	for (auto& block : m_blocks)
		block->SetPosition(position);
}

FLOAT HeightMapTerrain::GetHeight(FLOAT x, FLOAT z) const
{
	// 파라미터로 들어온 (x, z)는 플레이어의 위치이다.
	// (x, z)를 대응하는 이미지 좌표로 바꿔줘야한다.

	// 지형의 시작점 반영
	XMFLOAT3 pos{ GetPosition() };
	x -= pos.x;
	z -= pos.z;

	// 지형의 스케일 반영
	x /= m_scale.x;
	z /= m_scale.z;

	return pos.y + m_heightMapImage->GetHeight(x, z) * m_scale.y;
}

XMFLOAT3 HeightMapTerrain::GetNormal(FLOAT x, FLOAT z) const
{
	// 파라미터로 들어온 (x, z)는 플레이어의 위치이다.

	XMFLOAT3 pos{ GetPosition() };
	x -= pos.x; x /= m_scale.x;
	z -= pos.z; z /= m_scale.z;

	// (x, z) 주변의 노멀 4개를 보간해서 계산
	int ix{ static_cast<int>(x) };
	int iz{ static_cast<int>(z) };
	float fx{ x - ix };
	float fz{ z - iz };

	XMFLOAT3 LT{ m_heightMapImage->GetNormal(ix, iz + 1) };
	XMFLOAT3 LB{ m_heightMapImage->GetNormal(ix, iz) };
	XMFLOAT3 RT{ m_heightMapImage->GetNormal(ix + 1, iz + 1) };
	XMFLOAT3 RB{ m_heightMapImage->GetNormal(ix + 1, iz) };

	XMFLOAT3 bot{ Vector3::Add(Vector3::Mul(LB, 1.0f - fx), Vector3::Mul(RB, fx)) };
	XMFLOAT3 top{ Vector3::Add(Vector3::Mul(LT, 1.0f - fx), Vector3::Mul(RT, fx)) };
	return Vector3::Normalize(Vector3::Add(Vector3::Mul(bot, 1.0f - fz), Vector3::Mul(top, fz)));
}


Skybox::Skybox(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
	FLOAT width, FLOAT length, FLOAT depth) : GameObject()
{
	m_mesh = make_unique<SkyboxMesh>(device, commandList, width, length, depth);
}

void Skybox::Update(FLOAT timeElapsed)
{

}
