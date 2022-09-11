#pragma once
#include "stdafx.h"
#include "shader.h"
#include "object.h"
#include "player.h"
#include "camera.h"
#include "mesh.h"

class Scene
{
public:
	Scene() = default;
	~Scene();

	void BuildObjects(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandlist, const ComPtr<ID3D12RootSignature>& rootsignature, FLOAT	aspectRatio);
	void OnProcessingMouseMessage(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const;
	void OnProcessingKeyboardMessage(FLOAT timeElapsed) const;
	void Update(FLOAT timeElapsed);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	vector<unique_ptr<InstancingShader>>& GetShader() { return m_shader; }

private:
	vector<unique_ptr<InstancingShader>> m_shader;
};