#define MAX_MATERIAL 128

struct MATERIAL
{
	float4					diffuse;
	float4					emissive;
	float4					specular; //a = power
	float4					ambient;
};

cbuffer cbGameObject : register(b0)
{
	matrix worldMatrix : packoffset(c0);
	MATERIAL material : packoffset(c4);
};

cbuffer cbCamera : register(b1)
{
	matrix viewMatrix : packoffset(c0);
	matrix projMatrix : packoffset(c4);
};

/*
 *  STANDARD_SHADER 
 */
struct VS_STANDARD_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_STANDARD_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_STANDARD_OUTPUT VS_STANDARD_MAIN(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
}

[earlydepthstencil]
float4 PS_STANDARD_MAIN(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	input.color = material.diffuse + material.emissive + material.specular + material.ambient;
	//input.color = add(input.color, diffuse);
	//input.color = add(input.color, emissive);
	//input.color = add(input.color, specular);
	//input.color = add(input.color, ambient);
	return input.color;
}

/*
 *  INSTANCE_SHADER
 */
struct VS_INSTANCE_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	matrix worldMatrix : INSTANCE;
};

struct VS_INSTANCE_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_INSTANCE_OUTPUT VS_INSTANCE_MAIN(VS_INSTANCE_INPUT input)
{
	VS_INSTANCE_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), input.worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.color = input.color;
	return output;
}

[earlydepthstencil]
float4 PS_INSTANCE_MAIN(VS_INSTANCE_OUTPUT input) : SV_TARGET
{
	return input.color;
}

/*
 *  TERRAIN_SHADER
 */
Texture2D g_baseTexture : register(t0);
Texture2D g_detailTexture : register(t1);
SamplerState g_samplerState : register(s0);

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VS_TERRAIN_MAIN(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;
	return output;
}

[earlydepthstencil]
float4 PS_TERRAIN_MAIN(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 baseTexColor = g_baseTexture.Sample(g_samplerState, input.uv0);
	float4 detailTexColor = g_detailTexture.Sample(g_samplerState, input.uv1);

	return saturate((baseTexColor * 0.5f) + (detailTexColor * 0.5f));
}

/*
 *  SKYBOX_SHADER
 */
TextureCube g_skyboxTexture : register(t2);
SamplerState g_samplerClamp : register(s1);

struct VS_SKYBOX_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

VS_SKYBOX_OUTPUT VS_SKYBOX_MAIN(VS_SKYBOX_INPUT input)
{
	VS_SKYBOX_OUTPUT output;

	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix).xyww;
	output.positionL = input.position;

	return output;
}

[earlydepthstencil]
float4 PS_SKYBOX_MAIN(VS_SKYBOX_OUTPUT input) : SV_TARGET
{
	float4 color = g_skyboxTexture.Sample(g_samplerClamp, input.positionL);

	return color;
}