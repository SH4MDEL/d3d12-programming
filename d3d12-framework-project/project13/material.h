#pragma once
#include "stdafx.h"

// 재질(Material)
// 빛이 물체의 표면과 상호작용하는 방식을 결정하는 속성들의 집합
struct Material
{
	Material() : m_albedoColor{ XMFLOAT4(1.f, 1.f, 1.f, 1.f) }, m_emissiveColor{ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
		m_specularColor{ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) }, m_ambientColor{ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) } {}
	~Material() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	XMFLOAT4			m_albedoColor;
	XMFLOAT4			m_emissiveColor;
	XMFLOAT4			m_specularColor;
	XMFLOAT4			m_ambientColor;
};
