cbuffer cbGameObject : register(b0)
{
    matrix worldMatrix;
};

cbuffer cbCamera : register(b1)
{
    matrix viewMatrix;
    matrix projMatrix;
}

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    matrix worldMatrix : INSTANCE;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput result;
    result.position = mul(input.position, input.worldMatrix);
    result.position = mul(result.position, viewMatrix);
    result.position = mul(result.position, projMatrix);
    result.color = input.color;
    return result;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return input.color;
}