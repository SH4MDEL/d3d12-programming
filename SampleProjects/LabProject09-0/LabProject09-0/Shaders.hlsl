///////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtInput : register(t0);

RWTexture2D<float4> gtxtRWOutput : register(u0);
Texture2D gtxtOutput : register(t1);

SamplerState gSamplerState : register(s0);

static float3 gf3ToLuminance = float3(0.3f, 0.59f, 0.11f);

#define _WITH_BY_LUMINANCE

#define _WITH_GROUPSHARED_MEMORY
#define _WITH_SOBEL_EDGE

#define CX_THREADS	32
#define CY_THREADS	32

#ifdef _WITH_GROUPSHARED_MEMORY
groupshared float4 gf4GroupSharedCache[CX_THREADS+2][CY_THREADS+2];

//////////////////////////////////////////////////////////////////////////////////
// Sobel Edge Detection
//
void SobelEdge(int3 n3GroupThreadID : SV_GroupThreadID, int3 n3DispatchThreadID : SV_DispatchThreadID)
{
#ifdef _WITH_BY_LUMINANCE
	float fHorizontalEdge = (-1.0f * dot(gf3ToLuminance, gf4GroupSharedCache[n3GroupThreadID.x][n3GroupThreadID.y+1].rgb)) + (2.0f * dot(gf3ToLuminance, gf4GroupSharedCache[n3GroupThreadID.x+1][n3GroupThreadID.y+1].rgb)) + (-1.0f * dot(gf3ToLuminance, gf4GroupSharedCache[n3GroupThreadID.x+2][n3GroupThreadID.y+1].rgb));	
	float fVerticalEdge = (-1.0f * dot(gf3ToLuminance, gf4GroupSharedCache[n3GroupThreadID.x+1][n3GroupThreadID.y].rgb)) + (2.0f * dot(gf3ToLuminance, gf4GroupSharedCache[n3GroupThreadID.x+1][n3GroupThreadID.y+1].rgb)) + (-1.0f * dot(gf3ToLuminance, gf4GroupSharedCache[n3GroupThreadID.x+1][n3GroupThreadID.y+2].rgb));	
#else
	float3 f3HorizontalEdge = (-1.0f * gf4GroupSharedCache[n3GroupThreadID.x][n3GroupThreadID.y+1].rgb) + (2.0f * gf4GroupSharedCache[n3GroupThreadID.x+1][n3GroupThreadID.y+1].rgb) + (-1.0f * gf4GroupSharedCache[n3GroupThreadID.x+2][n3GroupThreadID.y+1].rgb);	
	float3 f3VerticalEdge = (-1.0f * gf4GroupSharedCache[n3GroupThreadID.x+1][n3GroupThreadID.y].rgb) + (2.0f * gf4GroupSharedCache[n3GroupThreadID.x+1][n3GroupThreadID.y+1].rgb) + (-1.0f * gf4GroupSharedCache[n3GroupThreadID.x+1][n3GroupThreadID.y+2].rgb);	
#endif
#ifdef _WITH_BY_LUMINANCE
	float3 cEdgeness = sqrt(fHorizontalEdge * fHorizontalEdge + fVerticalEdge * fVerticalEdge) * 1.3f;
	gtxtRWOutput[n3DispatchThreadID.xy] = float4(cEdgeness, 1.0f);
#else
	gtxtRWOutput[n3DispatchThreadID.xy] = float4(sqrt(f3HorizontalEdge*f3HorizontalEdge + f3VerticalEdge*f3VerticalEdge), 1.0f);
#endif
}

//////////////////////////////////////////////////////////////////////////////////
// Laplacian Edge Detection
//
static float gfLaplacians[9] = { -1.0f, -1.0f, -1.0f, -1.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f };
static int2 gnOffsets[9] = { { -1,-1 }, { 0,-1 }, { 1,-1 }, { -1,0 }, { 0,0 }, { 1,0 }, { -1,1 }, { 0,1 }, { 1,1 } };

void LaplacianEdge(int3 n3GroupThreadID : SV_GroupThreadID, int3 n3DispatchThreadID : SV_DispatchThreadID)
{
	float3 cEdgeness = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 9; i++)
	{
		cEdgeness += gfLaplacians[i] * dot(gf3ToLuminance, gf4GroupSharedCache[n3GroupThreadID.x+1+gnOffsets[i].x][n3GroupThreadID.y+1+gnOffsets[i].y].xyz);
	}

	gtxtRWOutput[n3DispatchThreadID.xy] = float4(cEdgeness, 1.0f);
}
#else
//////////////////////////////////////////////////////////////////////////////////
// Sobel Edge Detection
//
void SobelEdge(int3 n3DispatchThreadID : SV_DispatchThreadID)
{
#ifdef _WITH_BY_LUMINANCE
	float fHorizontalEdge = (-1.0f * dot(gf3ToLuminance, gtxtInput[int2(n3DispatchThreadID.x-1, n3DispatchThreadID.y)].rgb)) + (2.0f * dot(gf3ToLuminance, gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y)].rgb)) + (-1.0f * dot(gf3ToLuminance, gtxtInput[int2(n3DispatchThreadID.x+1, n3DispatchThreadID.y)].rgb));	
	float fVerticalEdge = (-1.0f * dot(gf3ToLuminance, gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y-1)].rgb)) + (2.0f * dot(gf3ToLuminance, gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y)].rgb)) + (-1.0f * dot(gf3ToLuminance, gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y+1)].rgb));	
#else
	float3 f3HorizontalEdge = (-1.0f * gtxtInput[int2(n3DispatchThreadID.x-1, n3DispatchThreadID.y)].rgb) + (2.0f * gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y)].rgb) + (-1.0f * gtxtInput[int2(n3DispatchThreadID.x+1, n3DispatchThreadID.y)].rgb);	
	float3 f3VerticalEdge = (-1.0f * gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y-1)].rgb) + (2.0f * gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y)].rgb) + (-1.0f * gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y+1)].rgb);	
#endif
#ifdef _WITH_BY_LUMINANCE
	float3 cEdgeness = sqrt(fHorizontalEdge * fHorizontalEdge + fVerticalEdge * fVerticalEdge);
	gtxtRWOutput[n3DispatchThreadID.xy] = float4(cEdgeness, 1.0f);
#else
	gtxtRWOutput[n3DispatchThreadID.xy] = float4(sqrt(f3HorizontalEdge*f3HorizontalEdge + f3VerticalEdge*f3VerticalEdge), 1.0f);
#endif
}

//////////////////////////////////////////////////////////////////////////////////
// Laplacian Edge Detection
//
static float gfLaplacians[9] = { -1.0f, -1.0f, -1.0f, -1.0f, 8.0f, -1.0f, -1.0f, -1.0f, -1.0f };
static int2 gnOffsets[9] = { { -1,-1 }, { 0,-1 }, { 1,-1 }, { -1,0 }, { 0,0 }, { 1,0 }, { -1,1 }, { 0,1 }, { 1,1 } };

void LaplacianEdge(int3 n3DispatchThreadID : SV_DispatchThreadID)
{
	float3 cEdgeness = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 9; i++)
	{
		cEdgeness += gfLaplacians[i] * dot(gf3ToLuminance, gtxtInput[int2(n3DispatchThreadID.xy) + gnOffsets[i]].xyz);
	}

	gtxtRWOutput[n3DispatchThreadID.xy] = float4(cEdgeness, 1.0f);
}
#endif

[numthreads(CX_THREADS, CY_THREADS, 1)]
void CSEdgeDetection(int3 n3GroupThreadID : SV_GroupThreadID, int3 n3DispatchThreadID : SV_DispatchThreadID)
{
#ifdef _WITH_GROUPSHARED_MEMORY
	gf4GroupSharedCache[n3GroupThreadID.x+1][n3GroupThreadID.y+1] = gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y)];
	if (n3GroupThreadID.x == 0) gf4GroupSharedCache[0][n3GroupThreadID.y+1] = gtxtInput[int2(n3DispatchThreadID.x-1, n3DispatchThreadID.y)];
	if (n3GroupThreadID.y == 0) gf4GroupSharedCache[n3GroupThreadID.x+1][0] = gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y-1)];
	if (n3GroupThreadID.x == CX_THREADS-1) gf4GroupSharedCache[CX_THREADS+1][n3GroupThreadID.y+1] = gtxtInput[int2(n3DispatchThreadID.x+1, n3DispatchThreadID.y)];
	if (n3GroupThreadID.y == CY_THREADS-1) gf4GroupSharedCache[n3GroupThreadID.x+1][CY_THREADS+1] = gtxtInput[int2(n3DispatchThreadID.x, n3DispatchThreadID.y+1)];
	if ((n3GroupThreadID.x == 0) && (n3GroupThreadID.y == 0)) gf4GroupSharedCache[0][0] = gtxtInput[int2(n3DispatchThreadID.x-1, n3DispatchThreadID.y-1)];
	else if ((n3GroupThreadID.x == CX_THREADS-1) && (n3GroupThreadID.y == 0)) gf4GroupSharedCache[CX_THREADS+1][0] = gtxtInput[int2(n3DispatchThreadID.x+1, n3DispatchThreadID.y-1)];
	else if ((n3GroupThreadID.x == 0) && (n3GroupThreadID.y == CY_THREADS-1)) gf4GroupSharedCache[0][CY_THREADS+1] = gtxtInput[int2(n3DispatchThreadID.x-1, n3DispatchThreadID.y+1)];
	else if ((n3GroupThreadID.x == CX_THREADS-1) && (n3GroupThreadID.y == CY_THREADS-1)) gf4GroupSharedCache[CX_THREADS+1][CY_THREADS+1] = gtxtInput[int2(n3DispatchThreadID.x+1, n3DispatchThreadID.y+1)];

	GroupMemoryBarrierWithGroupSync();

#ifdef _WITH_SOBEL_EDGE
	SobelEdge(n3GroupThreadID, n3DispatchThreadID);
#else
	LaplacianEdge(n3GroupThreadID, n3DispatchThreadID);
#endif
#else
#ifdef _WITH_SOBEL_EDGE
	SobelEdge(n3DispatchThreadID);
#else
	LaplacianEdge(n3DispatchThreadID);
#endif
#endif
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

float4 PSTextureToFullScreen(VS_TEXTURED_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtInput.Sample(gSamplerState, input.uv);
	float4 cEdgeColor = gtxtOutput.Sample(gSamplerState, input.uv) * 1.25f;

//	return(cEdgeColor);
	return(cColor * cEdgeColor);
//	return(cColor + cEdgeColor);
}
