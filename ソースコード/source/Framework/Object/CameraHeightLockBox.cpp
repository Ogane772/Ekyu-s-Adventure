#include "CameraHeightLockBox.h"
#include "../Collision/Box.h"
#include "../Scene/SceneManager.h"
#include "../Collision/Collision.h"
#include "../Camera/Camera.h"
#include "../../Game/Character/Player/Player.h"

CCameraHeightLockBox::CCameraHeightLockBox(const Vector3& SetPos, const Vector3& Max, const Vector3& Min)
{
	m_Position = SetPos;
	m_Scale = Vector3(1, 1, 1);
	m_Rotation = Vector3::Zero();
	m_Box = std::make_unique<CBox>(m_Position, Max, Min);
}

void CCameraHeightLockBox::Update(float DeltaTime)
{
	CPlayer* player = GetPlayer();
	if (!player)
	{
		return;
	}
	CCamera* camera = GetCamera();
	if (!camera)
	{
		return;
	}

	const bool isHit = CCollison::CollisionCheck(player->GetCapsule(), m_Box.get());
	//プレイヤーに非ヒット時からヒットした時
	if (isHit && !m_IsPlayerHit)
	{
		m_IsPlayerHit = true;
		camera->SetHeightLock(true);
		camera->SetHeightLockPosition(m_HeightLockPosition);
	}
	//プレイヤーにヒット時かつボックスからプレイヤーが離れたとき
	else if (!isHit && m_IsPlayerHit)
	{
		m_IsPlayerHit = false;
		camera->SetHeightLock(false);
	}
}

void CCameraHeightLockBox::Draw()
{
	if (!m_Box)
	{
		return;
	}
	m_Box->Draw();
}

void CCameraHeightLockBox::SingleCollisonDraw()
{
	if (!m_Box)
	{
		return;
	}
	m_Box->SingleDraw();
}

CBox* CCameraHeightLockBox::GetCollisionBox() const
{
	if (!m_Box)
	{
		return nullptr;
	}
	return m_Box.get();
}

void CCameraHeightLockBox::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	if (m_Box)
	{
		m_Box->SetPosition(m_Position);
	}
}
