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

	unique_ptr<InstancingShader> shader{ make_unique<InstancingShader>(device, rootsignature, cube, 1000) };

	// 게임오브젝트 생성
	for (int i = 0; i < 1000; ++i)
	{
		unique_ptr<RotatingObject> obj{ make_unique<RotatingObject>() };
		obj->SetPosition(XMFLOAT3(i % 10 * 5, (i / 10) % 10 * 5, (i / 100) % 10 * 5));
		obj->SetRotationSpeed((FLOAT)(i % 200));
		shader->GetGameObjects().push_back(move(obj));
	}

	// 플레이어 생성
	shared_ptr<Player> player{ make_shared<Player>() };
	player->SetPosition(XMFLOAT3(20.0f, 20.0f, 0.0f));
	shader->SetPlayer(player);

	// 카메라 생성
	shared_ptr<ThirdPersonCamera> camera{ make_shared<ThirdPersonCamera>() };
	camera->SetEye(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	camera->SetAt(XMFLOAT3{ 0.0f, 0.0f, 1.0f });
	camera->SetUp(XMFLOAT3{ 0.0f, 1.0f, 0.0f });
	camera->SetPlayer(shader->GetPlayer());

	// 플레이어 카메라 설정
	shader->GetPlayer()->SetCamera(shader->GetCamera());

	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, aspectRatio, 0.1f, 1000.0f));
	camera->SetProjMatrix(projMatrix);
	shader->SetCamera(camera);

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
