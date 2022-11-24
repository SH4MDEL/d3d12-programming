//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"

struct LIGHT
{
	XMFLOAT4					m_xmf4Ambient;
	XMFLOAT4					m_xmf4Diffuse;
	XMFLOAT4					m_xmf4Specular;
	XMFLOAT3					m_xmf3Position;
	float 						m_fFalloff;
	XMFLOAT3					m_xmf3Direction;
	float 						m_fTheta; //cos(m_fTheta)
	XMFLOAT3					m_xmf3Attenuation;
	float						m_fPhi; //cos(m_fPhi)
	bool						m_bEnable;
	int							m_nType;
	float						m_fRange;
	float						padding;
};								
								
struct LIGHTS					
{								
	LIGHT						m_pLights[MAX_LIGHTS];
	XMFLOAT4					m_xmf4GlobalAmbient;
};								
								
struct MATERIAL					
{								
	XMFLOAT4					m_xmf4Ambient;
	XMFLOAT4					m_xmf4Diffuse;
	XMFLOAT4					m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4					m_xmf4Emissive;
};								
								
struct MATERIALS				
{								
	MATERIAL					m_pReflections[MAX_MATERIALS];
};

class CScene
{
public:
    CScene();
    virtual ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, ID3D12GraphicsCommandList *pd3dCommandList);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam, ID3D12GraphicsCommandList *pd3dCommandList);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	bool ProcessInput(UCHAR *pKeysBuffer);
    void AnimateObjects(float fTimeElapsed);

	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	void ReleaseUploadBuffers();

	virtual ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	virtual ID3D12RootSignature* CreateComputeRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature *GetComputeRootSignature() { return(m_pd3dComputeRootSignature); }

	ID3D12RootSignature* CreateRootSignature(ID3D12Device* pd3dDevice, D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags, UINT nRootParameters, D3D12_ROOT_PARAMETER* pd3dRootParameters, UINT nStaticSamplerDescs, D3D12_STATIC_SAMPLER_DESC* pd3dStaticSamplerDescs);

protected:
	ID3D12RootSignature				*m_pd3dGraphicsRootSignature = NULL;
	ID3D12RootSignature				*m_pd3dComputeRootSignature = NULL;

protected:
	CGraphicsShader					**m_ppGraphicsShaders = NULL;
	int								m_nGraphicsShaders = 0;

	CComputeShader					**m_ppComputeShaders = NULL;
	int								m_nComputeShaders = 0;
};
