#pragma once
#include "stdafx.h"
#include "mesh.h"

class Shader
{
public:
	Shader() {}
	Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~Shader() = default;

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);

	ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_pipelineState; }

protected:
	ComPtr<ID3D12PipelineState>			m_pipelineState;
};

struct InstanceData
{
	XMFLOAT4X4 worldMatrix;
};

class InstancingShader : public Shader
{
public:
	InstancingShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature, const Mesh& mesh, UINT sizeofData, UINT count);
	~InstancingShader();

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature) override;
	void CreateInstancingBuffer(const ComPtr<ID3D12Device>& device);

	ComPtr<ID3D12Resource> GetInstancingBuffer() const { return m_instancingBuffer; }
	InstanceData* GetInstancingPointer() const { return m_instancingBufferPointer; }

	void Render(const ComPtr<ID3D12GraphicsCommandList>& m_commandList);

private:
	shared_ptr<Mesh>					m_mesh;
	
	UINT								m_sizeInBytes;
	UINT								m_strideInBytes;
	ComPtr<ID3D12Resource>				m_instancingBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_instancingBufferView;
	InstanceData*						m_instancingBufferPointer;
};