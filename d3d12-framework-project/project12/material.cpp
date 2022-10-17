#include "material.h"

void Material::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	commandList->SetGraphicsRoot32BitConstants(0, 4, &(m_albedoColor), 16);
	commandList->SetGraphicsRoot32BitConstants(0, 4, &(m_emissiveColor), 20);
	commandList->SetGraphicsRoot32BitConstants(0, 4, &(m_specularColor), 24);
	commandList->SetGraphicsRoot32BitConstants(0, 4, &(m_ambientColor), 28);
}