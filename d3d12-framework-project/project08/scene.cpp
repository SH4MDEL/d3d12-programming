#include "scene.h"


Scene::~Scene()
{
}

void Scene::OnProcessingMouseMessage(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const
{

}

void Scene::OnProcessingKeyboardMessage(FLOAT timeElapsed) const
{

}

void Scene::BuildObjects(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandlist, const ComPtr<ID3D12RootSignature>& rootsignature, FLOAT aspectRatio)
{
	unique_ptr<Shader> shader{ make_unique<Shader>(device, rootsignature) };

	// Render by index buffer
	vector<Vertex> vertices;
	vertices.emplace_back(XMFLOAT3{ -0.5f, +0.5f, +0.5f }, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +0.5f, +0.5f, +0.5f }, XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +0.5f, +0.5f, -0.5f }, XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ -0.5f, +0.5f, -0.5f }, XMFLOAT4{ 1.0f, 1.0f, 0.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ -0.5f, -0.5f, +0.5f }, XMFLOAT4{ 1.0f, 0.0f, 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +0.5f, -0.5f, +0.5f }, XMFLOAT4{ 0.0f, 1.0f, 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +0.5f, -0.5f, -0.5f }, XMFLOAT4{ 0.5f, 0.5f, 0.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ -0.5f, -0.5f, -0.5f }, XMFLOAT4{ 0.0f, 0.5f, 0.5f, 1.0f });

	vector<UINT> indices;
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(0); indices.push_back(2); indices.push_back(3);

	indices.push_back(3); indices.push_back(2); indices.push_back(6);
	indices.push_back(3); indices.push_back(6); indices.push_back(7);

	indices.push_back(7); indices.push_back(6); indices.push_back(5);
	indices.push_back(7); indices.push_back(5); indices.push_back(4);

	indices.push_back(1); indices.push_back(0); indices.push_back(4);
	indices.push_back(1); indices.push_back(4); indices.push_back(5);

	indices.push_back(0); indices.push_back(3); indices.push_back(7);
	indices.push_back(0); indices.push_back(7); indices.push_back(4);

	indices.push_back(2); indices.push_back(1); indices.push_back(5);
	indices.push_back(2); indices.push_back(5); indices.push_back(6);

	Mesh cube{ device, commandlist, vertices, indices };
	shared_ptr<Mesh> mesh{ make_shared<Mesh>(cube) };

	// 게임오브젝트 생성
	unique_ptr<RotatingObject> obj{ make_unique<RotatingObject>() };
	obj->SetPosition(XMFLOAT3{ 0.0f, -0.8f, 5.0f });
	obj->SetMesh(mesh);
	shader->GetGameObjects().push_back(move(obj));

	// 플레이어 생성
	shared_ptr<Player> player{ make_shared<Player>() };
	player->SetMesh(mesh);
	shader->SetPlayer(player);

	// 카메라 생성
	shared_ptr<ThirdPersonCamera> camera{ make_shared<ThirdPersonCamera>() };
	camera->SetEye(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	camera->SetAt(XMFLOAT3{ 0.0f, 0.0f, 1.0f });
	camera->SetUp(XMFLOAT3{ 0.0f, 1.0f, 0.0f });
	camera->SetPlayer(shader->GetPlayer());

	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, aspectRatio, 0.1f, 1000.0f));
	camera->SetProjMatrix(projMatrix);
	shader->SetCamera(camera);

	// 플레이어 카메라 설정
	shader->GetPlayer()->SetCamera(shader->GetCamera());

	m_shader.push_back(move(shader));
}

void Scene::Update(FLOAT timeElapsed)
{
	for (const auto& shader : m_shader)
		if (shader) shader->Update(timeElapsed);
}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	for (const auto& shader : m_shader) 
		if (shader) {
			shader->Render(commandList);
		}
}
