#pragma once
#include "Camera.h"

class CPlayer_Racing;

class CCamera_Racing : public CCamera
{
public:
	CCamera_Racing();
	CCamera_Racing(CCamera_Racing* pCamera);
	virtual ~CCamera_Racing();

	CPlayer_Racing* m_pPlayer;

	void SetPlayer(CPlayer_Racing* pPlayer) { m_pPlayer = pPlayer; }
	CPlayer_Racing* GetPlayer() { return(m_pPlayer); }
};

class CSpaceShipCamera_Racing : public CCamera_Racing
{
public:
	CSpaceShipCamera_Racing(CCamera_Racing* pCamera);
	virtual ~CSpaceShipCamera_Racing();

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CFirstPersonCamera_Racing : public CCamera_Racing
{
public:
	CFirstPersonCamera_Racing(CCamera_Racing* pCamera);
	virtual ~CFirstPersonCamera_Racing();

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CThirdPersonCamera_Racing : public CCamera_Racing
{
public:
	CThirdPersonCamera_Racing(CCamera_Racing* pCamera);
	virtual ~CThirdPersonCamera_Racing();

	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);
};



