cbuffer cbGameObject : register(b0)
{
	matrix worldMatrix : packoffset(c0);
};

cbuffer cbCamera : register(b1)
{
	matrix viewMatrix : packoffset(c0);
	matrix projMatrix : packoffset(c4);
};

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.color = input.color;
	return output;
}

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
	return input.color;
}


struct VS_INSTANCE_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	matrix worldMatrix : INSTANCE;
};

VS_OUTPUT VS_Instance_Main(VS_INSTANCE_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), input.worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.color = input.color;
	return output;
}

float4 PS_Instance_Main(VS_OUTPUT input) : SV_TARGET
{
	return input.color;
}


Texture2D g_baseTexture : register(t0);
Texture2D g_detailTexture : register(t1);
SamplerState g_samplerState : register(s0);

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};
//
struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VS_Terrain_Main(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;
	return output;
}

float4 PS_Terrain_Main(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 baseTexColor = g_baseTexture.Sample(g_samplerState, input.uv0);
	float4 detailTexColor = g_detailTexture.Sample(g_samplerState, input.uv1);

	return saturate((baseTexColor * 0.5f) + (detailTexColor * 0.5f));
}