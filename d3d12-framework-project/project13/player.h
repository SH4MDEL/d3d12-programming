#pragma once
#include "stdafx.h"
#include "object.h"

#define ROLL_MAX +20
#define ROLL_MIN -10

class Camera;

class Player : public Helicoptor
{
public:
	Player();
	~Player() = default;

	virtual void Update(FLOAT timeElapsed);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);

	void ApplyFriction(FLOAT deltaTime);
	void ApplyGravity(FLOAT deltaTime);

	XMFLOAT3 GetVelocity() const { return m_velocity; }
	shared_ptr<HeightMapTerrain> GetTerrain() const { return m_terrain; }

	void SetVelocity(const XMFLOAT3& velocity) { m_velocity = velocity; }
	void AddVelocity(const XMFLOAT3& increase);
	void SetCamera(const shared_ptr<Camera>& camera) { m_camera = camera; }
	void SetTerrain(const shared_ptr<HeightMapTerrain>& terrain) { m_terrain = terrain; }

private:
	XMFLOAT3						m_velocity;		// 속도
	FLOAT							m_maxVelocity;	// 최대속도
	FLOAT							m_friction;		// 마찰력

	shared_ptr<Camera>				m_camera;		// 카메라
	shared_ptr<HeightMapTerrain>	m_terrain;
};
