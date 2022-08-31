#pragma once
#include "stdafx.h"

class GameFramework
{
public:
	GameFramework(UINT width, UINT height);
	~GameFramework();

	void OnCreate(HINSTANCE hInstance, HWND hWnd);
	void BuildObjects();
	void OnDestroy();

	void CreateDevice();
	void CreateCommandQueue();
	void CreateSwapChain();
	void CreateRtvDsvDescriptorHeap();
	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();

private:
	static const INT					SwapChainBufferCount = 2;

	// Window
	HINSTANCE							m_hInstance;
	HWND								m_hWnd;
	UINT								m_width;
	UINT								m_height;

	D3D12_VIEWPORT						m_viewport;
	D3D12_RECT							m_scissorRect;
	ComPtr<IDXGIFactory4>				m_factory;
	ComPtr<IDXGISwapChain3>				m_swapChain;
	ComPtr<ID3D12Device>				m_device;
	INT									m_MSAA4xQualityLevel;
	ComPtr<ID3D12CommandAllocator>		m_commandAllocator;
	ComPtr<ID3D12CommandQueue>			m_commandQueue;
	ComPtr<ID3D12GraphicsCommandList>	m_commandList;
	ComPtr<ID3D12Resource>				m_renderTargets[SwapChainBufferCount];
	ComPtr<ID3D12DescriptorHeap>		m_rtvHeap;
	ComPtr<ID3D12Resource>				m_depthStencil;
	ComPtr<ID3D12DescriptorHeap>		m_dsvHeap;
	ComPtr<ID3D12PipelineState>			m_pipelineState;

	ComPtr<ID3D12Fence>					m_fence;
	UINT								m_frameIndex;
	UINT64								m_fenceValue;
	HANDLE								m_fenceEvent;

};

