//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameObjectInfo : register(b0)
{
	matrix worldMatrix : packoffset(c0);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCameraInfo : register(b1)
{
	matrix viewMatrix : packoffset(c0);
	matrix projMatrix : packoffset(c4);
};

//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
	matrix worldMatrix : INSTANCE;
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
	output.position = mul(input.position, input.worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projMatrix);
	output.color = input.color;
	return(output);
}

//픽셀 셰이더를 정의한다. 
float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}