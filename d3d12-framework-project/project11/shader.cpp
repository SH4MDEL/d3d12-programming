#include "shader.h"

Shader::Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature) : Shader{}
{
	ComPtr<ID3DBlob> mvsByteCode;
	ComPtr<ID3DBlob> mpsByteCode;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_STANDARD_MAIN", "vs_5_1", compileFlags, 0, &mvsByteCode, nullptr));
	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_STANDARD_MAIN", "ps_5_1", compileFlags, 0, &mpsByteCode, nullptr));

	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(mvsByteCode.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(mpsByteCode.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	DX::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));

}

Shader::~Shader()
{
	ReleaseShaderVariable();
}

void Shader::Update(FLOAT timeElapsed)
{
	if (m_player) m_player->Update(timeElapsed);

	for (const auto& elm : m_gameObjects)
		if (elm) elm->Update(timeElapsed);
}

void Shader::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	Shader::UpdateShaderVariable(commandList);

	if (m_player) m_player->Render(commandList);

	for (const auto& elm : m_gameObjects)
		if (elm) elm->Render(commandList);
}

void Shader::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	commandList->SetPipelineState(m_pipelineState.Get());
}

void Shader::ReleaseShaderVariable()
{

}

void Shader::SetPlayer(const shared_ptr<Player>& player)
{
	if (m_player) m_player.reset();
	m_player = player;
}

void Shader::SetCamera(const shared_ptr<Camera>& camera)
{
	if (m_camera) m_camera.reset();
	m_camera = camera;
}

TerrainShader::TerrainShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	CreatePipelineState(device, rootSignature);
}

void TerrainShader::CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	ComPtr<ID3DBlob> vertexShader, pixelShader;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_TERRAIN_MAIN", "vs_5_1", compileFlags, 0, &vertexShader, nullptr));
	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_TERRAIN_MAIN", "ps_5_1", compileFlags, 0, &pixelShader, nullptr));

	// 정점 셰이더 레이아웃 설정
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// PSO 생성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	DX::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void TerrainShader::Update(FLOAT timeElapsed)
{
	if (m_heightMap) m_heightMap->Update(timeElapsed);
}

void TerrainShader::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	UpdateShaderVariable(commandList);

	if (m_heightMap) { m_heightMap->Render(commandList); }
}


InstancingShader::InstancingShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature, const Mesh& mesh, UINT count) : 
	m_instancingCount(count)
{
	ComPtr<ID3DBlob> mvsByteCode;
	ComPtr<ID3DBlob> mpsByteCode;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_INSTANCE_MAIN", "vs_5_1", compileFlags, 0, &mvsByteCode, nullptr));
	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_INSTANCE_MAIN", "ps_5_1", compileFlags, 0, &mpsByteCode, nullptr));
	
	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}, 
		{ "INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		{ "INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(mvsByteCode.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(mpsByteCode.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	DX::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));

	CreateShaderVariable(device);
	m_mesh = make_unique<Mesh>(mesh);
}

void InstancingShader::Update(FLOAT timeElapsed)
{
	if (m_player) m_player->Update(timeElapsed);
	for (const auto& elm : m_gameObjects)
		if (elm) elm->Update(timeElapsed);
}

void InstancingShader::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	InstancingShader::UpdateShaderVariable(commandList);

	int i = 0;
	for (const auto& elm : m_gameObjects) {
		m_instancingBufferPointer[i++].worldMatrix = Matrix::Transpose(elm->GetWorldMatrix());
	}

	m_mesh->Render(commandList, m_instancingBufferView);
}

void InstancingShader::CreateShaderVariable(const ComPtr<ID3D12Device>& device)
{
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(InstancingData) * m_instancingCount),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&m_instancingBuffer)));

	// 인스턴스 버퍼 포인터
	m_instancingBuffer->Map(0, NULL, reinterpret_cast<void**>(&m_instancingBufferPointer));

	// 인스턴스 버퍼 뷰 생성
	m_instancingBufferView.BufferLocation = m_instancingBuffer->GetGPUVirtualAddress();
	m_instancingBufferView.StrideInBytes = sizeof(InstancingData);
	m_instancingBufferView.SizeInBytes = sizeof(InstancingData) * m_instancingCount;
}

void InstancingShader::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	commandList->SetPipelineState(m_pipelineState.Get());
}

void InstancingShader::ReleaseShaderVariable()
{
	if (m_instancingBuffer) m_instancingBuffer->Unmap(0, nullptr);
}

SkyboxShader::SkyboxShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	ComPtr<ID3DBlob> mvsByteCode;
	ComPtr<ID3DBlob> mpsByteCode;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_SKYBOX_MAIN", "vs_5_1", compileFlags, 0, &mvsByteCode, nullptr));
	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_SKYBOX_MAIN", "ps_5_1", compileFlags, 0, &mpsByteCode, nullptr));

	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(mvsByteCode.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(mpsByteCode.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;

	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	DX::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}