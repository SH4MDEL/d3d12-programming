#include "scene.h"


Scene::~Scene()
{
}

void Scene::OnProcessingMouseMessage(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const
{

}

void Scene::OnProcessingKeyboardMessage(FLOAT deltaTime) const
{

}

void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	for (const auto& object : m_gameObjects) object->Render(commandList);
}