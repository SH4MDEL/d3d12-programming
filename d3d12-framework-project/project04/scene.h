#pragma once
#include "stdafx.h"
#include "object.h"

class Scene
{
public:
	Scene() = default;
	~Scene();

	void OnProcessingMouseMessage(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const;
	void OnProcessingKeyboardMessage(FLOAT deltaTime) const;
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	vector<unique_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }

private:
	vector<unique_ptr<GameObject>>	m_gameObjects;
};