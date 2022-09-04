#pragma once
#include "stdafx.h"

class Scene
{
public:
	Scene() = default;
	~Scene();

	void OnProcessingMouseMessage(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const;
	void OnProcessingKeyboardMessage(FLOAT deltaTime) const;
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

private:
};

