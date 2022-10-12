#include "player.h"
#include "camera.h"

Player::Player() : HierarchyObject{}, m_velocity{ 0.0f, 0.0f, 0.0f }, m_maxVelocity{ 10.0f }, m_friction{ 0.5f }
{

}

void Player::Update(FLOAT timeElapsed)
{
	Move(m_velocity);
	ApplyGravity(timeElapsed);
	// 플레이어가 어떤 지형 위에 있다면
	if (m_terrain)
	{
		// 지형 아래로 이동할 수 없도록 보정
		XMFLOAT3 pos{ GetPosition() };
		XMFLOAT3 terrainScale{ m_terrain->GetScale() };
		FLOAT terrainHeight{ m_terrain->GetHeight(pos.x, pos.z) };

		if ((terrainHeight + 0.05f) / terrainScale.y > pos.y) {
			SetPosition(XMFLOAT3{ pos.x / terrainScale.x, (terrainHeight + 0.05f) / terrainScale.y, pos.z / terrainScale.z });
			m_velocity.y = 0.0f;
		}
	}

	ApplyFriction(timeElapsed);
}

void Player::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
	// 회전각 제한
	if (m_roll + roll > MAX_ROLL)
		roll = MAX_ROLL - m_roll;
	else if (m_roll + roll < MIN_ROLL)
		roll = MIN_ROLL - m_roll;

	// 회전각 합산
	m_roll += roll; m_pitch += pitch; m_yaw += yaw;

	// 카메라는 x,y축으로 회전할 수 있다.
	// GameObject::Rotate에서 플레이어의 로컬 x,y,z축을 변경하므로 먼저 호출해야한다.
	m_camera->Rotate(roll, pitch, 0.0f);

	// 플레이어는 y축으로만 회전할 수 있다.
	GameObject::Rotate(0.0f, pitch, 0.0f);
}

void Player::ApplyFriction(FLOAT deltaTime)
{
	FLOAT yvalue = m_velocity.y;
	m_velocity = Vector3::Mul(m_velocity, 1 / m_friction * deltaTime);
	m_velocity.y = yvalue;
}

void Player::ApplyGravity(FLOAT deltaTime)
{
	m_velocity.y = m_velocity.y + m_gravity * deltaTime;
}

void Player::AddVelocity(const XMFLOAT3& increase)
{
	m_velocity = Vector3::Add(m_velocity, increase);

	// 최대 속도에 걸린다면 해당 비율로 축소시킴
	FLOAT length{ Vector3::Length(m_velocity) };
	if (length > m_maxVelocity)
	{
		FLOAT ratio{ m_maxVelocity / length };
		m_velocity = Vector3::Mul(m_velocity, ratio);
	}
}