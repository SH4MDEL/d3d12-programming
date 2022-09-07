#include "shader.h"

Shader::Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	CreatePipelineState(device, rootSignature);
}

void Shader::CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	ComPtr<ID3DBlob> mvsByteCode;
	ComPtr<ID3DBlob> mpsByteCode;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_1", compileFlags, 0, &mvsByteCode, nullptr));
	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Shaders.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_1", compileFlags, 0, &mpsByteCode, nullptr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};
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

InstancingShader::InstancingShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature, const Mesh& mesh, UINT sizeofData, UINT count) : 
	m_instancingBufferView{}, 
	m_instancingBufferPointer{}, 
	m_sizeInBytes{ sizeofData * count }, 
	m_strideInBytes{ sizeofData }
{
	CreatePipelineState(device, rootSignature);
	CreateInstancingBuffer(device);
	m_mesh = make_shared<Mesh>(mesh);
}

InstancingShader::~InstancingShader()
{
	m_instancingBuffer->Unmap(0, NULL);
}

void InstancingShader::CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	ComPtr<ID3DBlob> mvsByteCode;
	ComPtr<ID3DBlob> mpsByteCode;

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Instance.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_1", compileFlags, 0, &mvsByteCode, nullptr));
	DX::ThrowIfFailed(D3DCompileFromFile(TEXT("Instance.hlsl"), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_1", compileFlags, 0, &mpsByteCode, nullptr));

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};
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

void InstancingShader::CreateInstancingBuffer(const ComPtr<ID3D12Device>& device)
{
	// 인스턴스 정보를 저장할 정점 버퍼를 업로드 힙 유형으로 생성한다.
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_sizeInBytes),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&m_instancingBuffer)));

	// 인스턴스 버퍼 포인터
	m_instancingBuffer->Map(0, NULL, reinterpret_cast<void**>(&m_instancingBufferPointer));

	// 인스턴스 버퍼 뷰 생성
	m_instancingBufferView.BufferLocation = m_instancingBuffer->GetGPUVirtualAddress();
	m_instancingBufferView.StrideInBytes = m_strideInBytes;
	m_instancingBufferView.SizeInBytes = m_sizeInBytes;
}

void InstancingShader::Render(const ComPtr<ID3D12GraphicsCommandList>& m_commandList)
{
	if (m_mesh)
	{
		m_commandList->SetPipelineState(m_pipelineState.Get());
		m_mesh->Render(m_commandList, m_instancingBufferView);
	}
}