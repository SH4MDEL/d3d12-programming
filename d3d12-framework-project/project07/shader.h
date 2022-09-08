#pragma once
#include "stdafx.h"
#include "object.h"
#include "player.h"
#include "camera.h"

class Shader
{
public:
	Shader();
	Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~Shader() = default;

	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void Update(FLOAT timeElapsed);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	shared_ptr<Player> GetPlayer() const { return m_player; }
	shared_ptr<Camera> GetCamera() const { return m_camera; }
	vector<unique_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_pipelineState; }

	void SetPlayer(const shared_ptr<Player>& player);
	void SetCamera(const shared_ptr<Camera>& camera);

protected:
	ComPtr<ID3D12PipelineState>			m_pipelineState;
	vector<D3D12_INPUT_ELEMENT_DESC>	m_inputLayout;

	vector<unique_ptr<GameObject>>		m_gameObjects;

	shared_ptr<Player>					m_player;
	shared_ptr<Camera>					m_camera;
};

