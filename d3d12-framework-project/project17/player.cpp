#include "player.h"
#include "camera.h"

Player::Player() : Helicoptor{}, m_velocity{ 0.0f, 0.0f, 0.0f }, m_maxVelocity{ 10.0f }, m_friction{ 0.5f }, m_missileState{READY}
{

}

void Player::Update(FLOAT timeElapsed)
{
	Helicoptor::Update(timeElapsed);
	Move(m_velocity);
	// �÷��̾ � ���� ���� �ִٸ�
	if (m_terrain) {
		// ���� �Ʒ��� �̵��� �� ������ ����
		XMFLOAT3 pos{ GetPosition() };
		XMFLOAT3 terrainScale{ m_terrain->GetScale() };
		FLOAT terrainHeight{ m_terrain->GetHeight(pos.x, pos.z) };

		if ((terrainHeight + 0.05f) / terrainScale.y > pos.y) {
			SetPosition(XMFLOAT3{ pos.x / terrainScale.x, (terrainHeight + 0.05f) / terrainScale.y, pos.z / terrainScale.z });
			m_velocity.y = 0.0f;
		}
	}
	if (m_missileState == SHOTTING) {
		m_missileTimer += timeElapsed;

		m_missileFrame->SetWorldMatrix(m_missileWorld);

		m_missileFrame->Move(Vector3::Mul(m_missileVector, 30.0f * timeElapsed));
		m_missileWorld = m_missileFrame->GetWorldMatrix();
		
		XMFLOAT3 pos{ m_missileFrame->GetPosition() };
		XMFLOAT3 terrainScale{ m_terrain->GetScale() };
		FLOAT terrainHeight{ m_terrain->GetHeight(pos.x, pos.z) };

		m_missileFrame->UpdateBoundingBox();

		if (m_missileTimer >= m_missileLifeTime) {
			m_missileState = READY;
			m_missileTimer = 0.f;
		}

		if (terrainHeight / terrainScale.y > pos.y) {
			m_missileState = READY;
			m_missileTimer = 0.f;
		}
	}
	//cout << GetPosition().x << ", " << GetPosition().y << ", " << GetPosition().z << endl;
	ApplyFriction(timeElapsed);
}

void Player::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{

	// ȸ���� ����
	if (g_firstPerson) {
		if (m_roll + roll > FIRST_MAX_ROLL)
			roll = FIRST_MAX_ROLL - m_roll;
		else if (m_roll + roll < FIRST_MIN_ROLL)
			roll = FIRST_MIN_ROLL - m_roll;
	}
	else {
		if (m_roll + roll > THIRD_MAX_ROLL)
			roll = THIRD_MAX_ROLL - m_roll;
		else if (m_roll + roll < THIRD_MIN_ROLL)
			roll = THIRD_MIN_ROLL - m_roll;
	}


	// ȸ���� �ջ�
	m_roll += roll; m_pitch += pitch; m_yaw += yaw;

	// ī�޶�� x,y������ ȸ���� �� �ִ�.
	// GameObject::Rotate���� �÷��̾��� ���� x,y,z���� �����ϹǷ� ���� ȣ���ؾ��Ѵ�.
	m_camera->Rotate(roll, pitch, 0.0f);
	m_firstCamera->Rotate(roll, pitch, 0.0f);

	// �÷��̾�� y�����θ� ȸ���� �� �ִ�.
	GameObject::Rotate(0.0f, pitch, 0.0f);
}

void Player::ApplyFriction(FLOAT deltaTime)
{
	m_velocity = Vector3::Mul(m_velocity, 1 / m_friction * deltaTime);
}

void Player::AddVelocity(const XMFLOAT3& increase)
{
	m_velocity = Vector3::Add(m_velocity, increase);

	// �ִ� �ӵ��� �ɸ��ٸ� �ش� ������ ��ҽ�Ŵ
	FLOAT length{ Vector3::Length(m_velocity) };
	if (length > m_maxVelocity)
	{
		FLOAT ratio{ m_maxVelocity / length };
		m_velocity = Vector3::Mul(m_velocity, ratio);
	}
}

void Player::LaunchMissile()
{
	if (m_missileState == READY) {
		m_missileState = SHOTTING;
		m_missileVector = GetFront();
		m_missileWorld = m_worldMatrix;
	}
}
