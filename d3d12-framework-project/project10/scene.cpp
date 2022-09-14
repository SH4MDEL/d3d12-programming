#include "scene.h"


Scene::~Scene()
{
}

void Scene::OnProcessingMouseMessage(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const
{
	SetCursor(NULL);
	RECT rect; GetWindowRect(hWnd, &rect);
	POINT oldMousePosition{ rect.left + width / 2, rect.top + height / 2 };

	// 움직인 마우스 좌표
	POINT newMousePosition; GetCursorPos(&newMousePosition);

	// 움직인 정도에 비례해서 회전
	int dx = newMousePosition.x - oldMousePosition.x;
	int dy = newMousePosition.y - oldMousePosition.y;
	m_player->Rotate(dy * 5.0f * deltaTime, dx * 5.0f * deltaTime, 0.0f);
	SetCursorPos(oldMousePosition.x, oldMousePosition.y);
}

void Scene::OnProcessingKeyboardMessage(FLOAT timeElapsed) const
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetFront(), timeElapsed * 10.0f));
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetRight(), timeElapsed * -10.0f));
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetFront(), timeElapsed * -10.0f));
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetRight(), timeElapsed * 10.0f));
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetUp(), timeElapsed * 10.0f));
	}
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		m_player->AddVelocity(Vector3::Mul(m_player->GetUp(), timeElapsed * -10.0f));
	}
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

	unique_ptr<InstancingShader> instancingShader{ make_unique<InstancingShader>(device, rootsignature, cube, 1000) };

	// 게임오브젝트 생성
	for (int i = 0; i < 1000; ++i)
	{
		unique_ptr<RotatingObject> obj{ make_unique<RotatingObject>() };
		obj->SetPosition(XMFLOAT3(i % 10 * 5, (i / 10) % 10 * 5, (i / 100) % 10 * 5));
		obj->SetRotationSpeed((FLOAT)(i % 200));
		instancingShader->GetGameObjects().push_back(move(obj));
	}

	unique_ptr<Shader> playerShader{ make_unique<Shader>(device, rootsignature) };

	// 플레이어 생성
	m_player = make_shared<Player>();
	m_player->SetPosition(XMFLOAT3(20.0f, 20.0f, -10.0f));
	m_player->SetMesh(Mesh(device, commandlist, vertices, indices));
	playerShader->SetPlayer(m_player);

	// 카메라 생성
	m_camera = make_shared<ThirdPersonCamera>();
	m_camera->SetEye(XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	m_camera->SetAt(XMFLOAT3{ 0.0f, 0.0f, 1.0f });
	m_camera->SetUp(XMFLOAT3{ 0.0f, 1.0f, 0.0f });
	m_camera->SetPlayer(m_player);
	m_player->SetCamera(m_camera);

	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(0.25f * XM_PI, aspectRatio, 0.1f, 1000.0f));
	m_camera->SetProjMatrix(projMatrix);


	// 셰이더 설정
	m_shader.insert(make_pair("INSTANCING", move(instancingShader)));
	m_shader.insert(make_pair("PLAYER", move(playerShader)));
}

void Scene::Update(FLOAT timeElapsed)
{
	m_camera->Update(timeElapsed);
	for (const auto& shader : m_shader)
		shader.second->Update(timeElapsed);
}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (m_camera) m_camera->UpdateShaderVariable(commandList);
	for (const auto& shader : m_shader)
		shader.second->Render(commandList);
}
