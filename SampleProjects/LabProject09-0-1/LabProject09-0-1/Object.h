//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "Camera.h"

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

class CGraphicsShader;
class CGameObject;

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	UINT							m_nObjectID;
	UINT							m_nMaterialID;
};

class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nGraphicsSrvRootParameters, int nComputeUavRootParameters, int nComputeSrvRootParameters, int nGraphicsSrvGpuHandles, int nComputeUavGpuHandles, int nComputeSrvGpuHandles);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;

	int								m_nTextures = 0;
	_TCHAR							(*m_ppstrTextureNames)[64] = NULL;
	ID3D12Resource**				m_ppd3dTextures = NULL;
	ID3D12Resource**				m_ppd3dTextureUploadBuffers;

	UINT*							m_pnResourceTypes = NULL;

	DXGI_FORMAT*					m_pdxgiBufferFormats = NULL;
	int*							m_pnBufferElements = NULL;

	int								m_nGraphicsSrvGpuHandles = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE*	m_pd3dGraphicsSrvGpuDescriptorHandles = NULL;
	int								m_nComputeUavGpuHandles = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE*	m_pd3dComputeUavGpuDescriptorHandles = NULL;
	int								m_nComputeSrvGpuHandles = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE*	m_pd3dComputeSrvGpuDescriptorHandles = NULL;

	int								m_nGraphicsSrvRootParameters = 0; //Number of Graphics Root Parameters
	int*							m_pnGraphicsSrvRootParameterIndices = NULL; //Graphics Root Parameters Index of Root Signature
	int*							m_pnGraphicsSrvRootParameterDescriptors = NULL; //Number of Descriptors Per Graphics Root Parameter
	D3D12_GPU_DESCRIPTOR_HANDLE*	m_pd3dGraphicsSrvRootParameterGpuDescriptorHandles = NULL; //Start SRV Descriptor for Graphics Root Parameters

	int								m_nComputeUavRootParameters = 0;
	int*							m_pnComputeUavRootParameterIndices = NULL;
	int*							m_pnComputeUavRootParameteDescriptors = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE*	m_pd3dComputeUavRootParameterGpuDescriptorHandles = NULL;

	int								m_nComputeSrvRootParameters = 0;
	int*							m_pnComputeSrvRootParameterIndices = NULL;
	int*							m_pnComputeSrvRootParameterDescriptors = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE*	m_pd3dComputeSrvRootParameterGpuDescriptorHandles = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE*	m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateComputeShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateGraphicsShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateGraphicsSrvShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nIndex);
	void UpdateComputeSrvShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nIndex);
	void UpdateComputeUavShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	void LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex);

	void SetGraphicsSrvGpuDescriptorHandle(int nHandleIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGraphicsSrvGpuDescriptorHandle(int nHandleIndex) { return(m_pd3dGraphicsSrvGpuDescriptorHandles[nHandleIndex]); }

	void SetComputeUavGpuDescriptorHandle(int nHandleIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dUavGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetComputeUavGpuDescriptorHandle(int nHandleIndex) { return(m_pd3dComputeUavGpuDescriptorHandles[nHandleIndex]); }

	void SetComputeSrvGpuDescriptorHandle(int nHandleIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetComputeSrvGpuDescriptorHandle(int nHandleIndex) { return(m_pd3dComputeSrvGpuDescriptorHandles[nHandleIndex]); }

	int GetGraphicsSrvRootParameters() { return(m_nGraphicsSrvRootParameters); }
	void SetGraphicsSrvRootParameter(int nIndex, int nRootParameterIndex, int nGpuHandleIndex, int nSrvDescriptors);
	int GetGraphicsSrvRootParameterIndex(int nIndex) { return(m_pnGraphicsSrvRootParameterIndices[nIndex]); }

	int GetComputeSrvRootParameters() { return(m_nComputeSrvRootParameters); }
	void SetComputeSrvRootParameter(int nIndex, int nRootParameterIndex, int nGpuHandleIndex, int nSrvDescriptors);
	int GetComputeSrvRootParameterIndex(int nIndex) { return(m_pnComputeSrvRootParameterIndices[nIndex]); }

	int GetComputeUavRootParameters() { return(m_nComputeUavRootParameters); }
	void SetComputeUavRootParameter(int nIndex, int nRootParameterIndex, int nGpuHandleIndex, int nUavDescriptors);
	int GetComputeUavRootParameterIndex(int nIndex) { return(m_pnComputeUavRootParameterIndices[nIndex]); }

	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);
	D3D12_UNORDERED_ACCESS_VIEW_DESC GetUnorderedAccessViewDesc(int nIndex);

	void ReleaseUploadBuffers();
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4						m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	UINT							m_nReflection = 0;

	CTexture						*m_pTexture = NULL;
	CGraphicsShader					*m_pShader = NULL;

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetTexture(CTexture *pTexture);
	void SetShader(CGraphicsShader *pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
public:
	CGameObject(int nMeshes=1);
	virtual ~CGameObject();

public:
	XMFLOAT4X4						m_xmf4x4World;

	CMesh							**m_ppMeshes;
	int								m_nMeshes;

	CMaterial						*m_pMaterial = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

protected:
	ID3D12Resource					*m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO				*m_pcbMappedGameObject = NULL;

public:
	void SetMesh(int nIndex, CMesh *pMesh);
	void SetShader(CGraphicsShader *pShader);
	void SetMaterial(CMaterial *pMaterial);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Animate(float fTimeElapsed);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	virtual void BuildMaterials(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes=1);
	virtual ~CRotatingObject();

private:
	XMFLOAT3					m_xmf3RotationAxis;
	float						m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject(int nMeshes=1);
	virtual ~CRevolvingObject();

private:
	XMFLOAT3					m_xmf3RevolutionAxis;
	float						m_fRevolutionSpeed;

public:
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	void SetRevolutionAxis(XMFLOAT3 xmf3RevolutionAxis) { m_xmf3RevolutionAxis = xmf3RevolutionAxis; }

	virtual void Animate(float fTimeElapsed);
};

