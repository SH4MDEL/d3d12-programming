///////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtInputA : register(t0);
Texture2D gtxtInputB : register(t1);

RWTexture2D<float4> gtxtRWOutput : register(u0);

[numthreads(32, 32, 1)]
void CSAddTextures(int3 nDispatchID : SV_DispatchThreadID)
{
//	gtxtRWOutput[nDispatchID.xy] = gtxtInputA[nDispatchID.xy] + gtxtInputB[nDispatchID.xy];
	gtxtRWOutput[nDispatchID.xy] = lerp(gtxtInputA[nDispatchID.xy], gtxtInputB[nDispatchID.xy], 0.35f);
}

///////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextureToFullScreen(uint nVertexID : SV_VertexID)
{
	VS_TEXTURED_OUTPUT output;
	if (nVertexID == 0) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	if (nVertexID == 1) { output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
	if (nVertexID == 2) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	if (nVertexID == 3) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	if (nVertexID == 4) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	if (nVertexID == 5) { output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	return(output);
}

Texture2D gtxtOutput : register(t2);
SamplerState gSamplerState : register(s0);

float4 PSTextureToFullScreen(VS_TEXTURED_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtOutput.Sample(gSamplerState, input.uv);

	return(cColor);
}
