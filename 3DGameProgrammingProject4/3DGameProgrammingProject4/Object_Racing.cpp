#include "stdafx.h"
#include "Object_Racing.h"
#include "Shader_Racing.h"

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CShader_Racing* CMaterial_Racing::m_pPseudoLightingShader = NULL;

CMaterial_Racing::CMaterial_Racing()
{
}

CMaterial_Racing::~CMaterial_Racing()
{
	if (m_pShader) m_pShader->Release();
}

void CMaterial_Racing::SetShader(CShader_Racing* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial_Racing::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CMaterial_Racing::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pPseudoLightingShader = new CPseudoLightingShader();
	m_pPseudoLightingShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPseudoLightingShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}