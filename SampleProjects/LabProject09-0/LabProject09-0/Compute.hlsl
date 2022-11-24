
static float gfSofteningSquared	= 0.0012500000f * 0.0012500000f;
static float gfParticleMass	= (6.67300e-11f * 10000.0f) * 10000.0f * 10000.0f;

#define BLOCK_SIZE		128

groupshared float4 gpfGroupSharedPositions[BLOCK_SIZE];

void BodyToBodyInteraction(inout float3 ai, float4 bj, float4 bi, float fMass, int nParticles)
{
	float3 r = bj.xyz - bi.xyz;

	float fDistanceSqr = dot(r, r);
	fDistanceSqr += gfSofteningSquared;

	float fInverseDistance = 1.0f / sqrt(fDistanceSqr);
	float s = fMass * (fInverseDistance * fInverseDistance * fInverseDistance) * nParticles;

	ai += r * s;
}

cbuffer cbCS : register(b0)
{
	uint4 gu4Parameter;	// param[0] = MAX_PARTICLES, param[1] = dimx;
	float4 gf4Parameter;	// paramf[0] = 0.1f(deltaTime), paramf[1] = 1(damping); 
};

struct POSITIONVELOCITY
{
	float4 position;
	float4 velocity;
};

StructuredBuffer<POSITIONVELOCITY> gsbOldPositionVelocity : register(t0);	
RWStructuredBuffer<POSITIONVELOCITY> gsbNewPositionVelocity	: register(u0);	

[numthreads(BLOCK_SIZE, 1, 1)]
void CSMain(uint3 nGroupID : SV_GroupID, uint3 nDispatchThreadID : SV_DispatchThreadID, uint3 nGroupThreadID : SV_GroupThreadID, uint nGroupIndex : SV_GroupIndex)
{
	float4 position = gsbOldPositionVelocity[nDispatchThreadID.x].position;
	float4 velocity = gsbOldPositionVelocity[nDispatchThreadID.x].velocity;
	float3 acceleration = 0;

	[loop]
	for (uint tile = 0; tile < gu4Parameter.y; tile++)
	{
		gpfGroupSharedPositions[nGroupIndex] = gsbOldPositionVelocity[tile * BLOCK_SIZE + nGroupIndex].position;
		GroupMemoryBarrierWithGroupSync();

		[unroll]
		for (uint i = 0; i < BLOCK_SIZE; i += 8)
		{
			BodyToBodyInteraction(acceleration, gpfGroupSharedPositions[i], position, gfParticleMass, 1);
			BodyToBodyInteraction(acceleration, gpfGroupSharedPositions[i+1], position, gfParticleMass, 1);
			BodyToBodyInteraction(acceleration, gpfGroupSharedPositions[i+2], position, gfParticleMass, 1);
			BodyToBodyInteraction(acceleration, gpfGroupSharedPositions[i+3], position, gfParticleMass, 1);
			BodyToBodyInteraction(acceleration, gpfGroupSharedPositions[i+4], position, gfParticleMass, 1);
			BodyToBodyInteraction(acceleration, gpfGroupSharedPositions[i+5], position, gfParticleMass, 1);
			BodyToBodyInteraction(acceleration, gpfGroupSharedPositions[i+6], position, gfParticleMass, 1);
			BodyToBodyInteraction(acceleration, gpfGroupSharedPositions[i+7], position, gfParticleMass, 1);
		}

		GroupMemoryBarrierWithGroupSync();
	}

	const int nTooManyParticles = gu4Parameter.y * BLOCK_SIZE - gu4Parameter.x;
	BodyToBodyInteraction(acceleration, float4(0, 0, 0, 0), position, gfParticleMass, -nTooManyParticles);

	velocity.xyz += acceleration.xyz * gf4Parameter.x;		
	velocity.xyz *= gf4Parameter.y;					
	position.xyz += velocity.xyz * gf4Parameter.x;		

	if (nDispatchThreadID.x < gu4Parameter.x)
	{
		gsbNewPositionVelocity[nDispatchThreadID.x].position = position;
		gsbNewPositionVelocity[nDispatchThreadID.x].velocity = float4(velocity.xyz, length(acceleration));
	}
}
