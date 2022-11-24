///////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtInput : register(t0);

RWTexture2D<float4> gtxtRWOutput : register(u0);

SamplerState gSamplerState : register(s0);

static float3 gf3ToLuminance = float3(0.3f, 0.59f, 0.11f);

#define _WITH_2D_GAUSSIAN_BLUR
#define _WITH_GROUPSHARED_MEMORY

#ifdef _WITH_2D_GAUSSIAN_BLUR
groupshared float4 gf4GroupSharedCache[2+32+2][2+32+2];

static float gfGaussianBlurMask2D[5][5] = { 
	{ 1.0f/273.0f, 4.0f/273.0f, 7.0f/273.0f, 4.0f/273.0f, 1.0f/273.0f },
	{ 4.0f/273.0f, 16.0f/273.0f, 26.0f/273.0f, 16.0f/273.0f, 4.0f/273.0f },
	{ 7.0f/273.0f, 26.0f/273.0f, 41.0f/273.0f, 26.0f/273.0f, 7.0f/273.0f },
	{ 4.0f/273.0f, 16.0f/273.0f, 26.0f/273.0f, 16.0f/273.0f, 4.0f/273.0f },
	{ 1.0f/273.0f, 4.0f/273.0f, 7.0f/273.0f, 4.0f/273.0f, 1.0f/273.0f } 
};

[numthreads(32, 32, 1)]
void CSGaussian2DBlur(int3 n3GroupThreadID : SV_GroupThreadID, int3 n3DispatchThreadID : SV_DispatchThreadID)
{
	if ((n3DispatchThreadID.x < 2) || (n3DispatchThreadID.x >= int(gtxtInput.Length.x - 2)) || (n3DispatchThreadID.y < 2) || (n3DispatchThreadID.y >= int(gtxtInput.Length.y - 2))) 
	{
		gtxtRWOutput[n3DispatchThreadID.xy] = gtxtInput[n3DispatchThreadID.xy];
	}
	else
	{
		float4 f4Color = float4(0, 0, 0, 0);
		for (int i = -2; i <= 2; i++) 
		{
			for (int j = -2; j <= 2; j++) 
			{
				f4Color += gfGaussianBlurMask2D[i+2][j+2] * gtxtInput[n3DispatchThreadID.xy + int2(i, j)];	
			}
		}
		gtxtRWOutput[n3DispatchThreadID.xy] = f4Color;
	}
}
#else
static float gfGaussianBlurMask1D[11] = { 0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f };

groupshared float4 gf4GroupSharedCache[256 + 5 + 5];

[numthreads(256, 1, 1)]
void CSHorizontalBlur(int3 n3GroupThreadID : SV_GroupThreadID, int3 n3DispatchThreadID : SV_DispatchThreadID)
{
	if (n3GroupThreadID.x < 5)
	{
		int x = max(n3DispatchThreadID.x - 5, 0);
		gf4GroupSharedCache[n3GroupThreadID.x] = gtxtInput[int2(x, n3DispatchThreadID.y)];
	}
	else if (n3GroupThreadID.x >= (256 - 5))
	{
		int x = min(n3DispatchThreadID.x + 5, gtxtInput.Length.x - 1);
		gf4GroupSharedCache[n3GroupThreadID.x + (2 * 5)] = gtxtInput[int2(x, n3DispatchThreadID.y)];
	}
	gf4GroupSharedCache[n3GroupThreadID.x + 5] = gtxtInput[min(n3DispatchThreadID.xy, gtxtInput.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 vColor = float4(0, 0, 0, 0);
	for (int i = -5; i <= 5; i++) vColor += gfGaussianBlurMask1D[i + 5] * gf4GroupSharedCache[n3GroupThreadID.x + 5 + i];

	gtxtRWOutput[n3DispatchThreadID.xy] = vColor;
}

[numthreads(1, 256, 1)]
void CSVerticalBlur(int3 n3GroupThreadID : SV_GroupThreadID, int3 n3DispatchThreadID : SV_DispatchThreadID)
{
	if (n3GroupThreadID.y < 5) 
	{
		int y = max(n3DispatchThreadID.y - 5, 0);
		gf4GroupSharedCache[n3GroupThreadID.y] = gtxtInput[int2(n3DispatchThreadID.x, y)];
	}
	else if (n3GroupThreadID.y >= 256 - 5)
	{
		int y = min(n3DispatchThreadID.y + 5, gtxtInput.Length.y - 1);
		gf4GroupSharedCache[n3GroupThreadID.y + (2 * 5)] = gtxtInput[int2(n3DispatchThreadID.x, y)];
	}	
	gf4GroupSharedCache[n3GroupThreadID.y + 5] = gtxtInput[min(n3DispatchThreadID.xy, gtxtInput.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 vColor = float4(0, 0, 0, 0);
	for (int i = -5; i <= 5; i++) vColor += gfGaussianBlurMask1D[i + 5] * gf4GroupSharedCache[n3GroupThreadID.y + 5 + i];

	gtxtRWOutput[n3DispatchThreadID.xy] = vColor;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtOutput : register(t1);

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

float4 PSTextureToFullScreen(VS_TEXTURED_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtInput.Sample(gSamplerState, input.uv);
	float4 cEdgeColor = gtxtOutput.Sample(gSamplerState, input.uv) * 1.25f;

	return(cEdgeColor);
//	return(cColor * cEdgeColor);
//	return(cColor + cEdgeColor);
}
