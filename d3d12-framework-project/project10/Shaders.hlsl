//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameObject : register(b0)
{
	matrix worldMatrix : packoffset(c0);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCamera : register(b1)
{
	matrix viewMatrix : packoffset(c0);
	matrix projMatrix : packoffset(c4);
};

//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_INSTANCE_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	matrix worldMatrix : INSTANCE;
};

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 normal : NORMAL;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다. 
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//정점 셰이더를 정의한다. 
VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.color = input.color;
	return(output);
}

VS_OUTPUT VS_Instance_Main(VS_INSTANCE_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), input.worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.color = input.color;
	return(output);
}

VS_OUTPUT VS_Terrain_Main(VS_TERRAIN_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.color = mul(float4(0.2f, 0.2f, 0.2f, 0.0f), input.normal);
	return(output);
}

//픽셀 셰이더를 정의한다. 
float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

float4 PS_Instance_Main(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

float4 PS_Terrain_Main(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}