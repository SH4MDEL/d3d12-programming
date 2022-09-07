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

void Scene::Update(FLOAT timeElapsed)
{
	if (m_player) m_player->Update(timeElapsed);
	for (const auto& object : m_gameObjects) object->Update(timeElapsed);
}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (m_camera) m_camera->UpdateShaderVariable(commandList);
	if (m_player) m_player->Render(commandList);

	if (m_shader) {
		InstanceData* inst{ m_shader->GetInstancingPointer() };

		int i = 0;
		for (const auto& obj : m_gameObjects)
			inst[i++].worldMatrix = Matrix::Transpose(obj->GetWorldMatrix());

		m_shader->Render(commandList);
	}
	else {
		for (const auto& object : m_gameObjects) object->Render(commandList);
	}
}

void Scene::SetPlayer(const shared_ptr<Player>& player)
{
	if (m_player) m_player.reset();
	m_player = player;
}

void Scene::SetCamera(const shared_ptr<Camera>& camera)
{
	if (m_camera) m_camera.reset();
	m_camera = camera;
}

void Scene::SetShader(const shared_ptr<InstancingShader>& shader)
{
	if (m_shader) m_shader.reset();
	m_shader = shader;
}