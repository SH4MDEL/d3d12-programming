#pragma once
#include "stdafx.h"

class Psysics
{
public:
	Psysics();
	~Psysics();
private:
	const FLOAT						m_gravity = -10.0f;		// 중력
	XMFLOAT3						m_velocity;		// 속도
	XMFLOAT3						m_acceleration;	// 가속도
	FLOAT							m_maxVelocity;	// 최대속도
	FLOAT							m_friction;		// 마찰력
};

