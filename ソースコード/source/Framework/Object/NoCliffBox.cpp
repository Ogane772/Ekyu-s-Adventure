#include "NoCliffBox.h"
#include "../Collision/Box.h"
#include "../Scene/SceneManager.h"
#include "../Collision/Collision.h"
#include "../../Game/Character/Player/Player.h"

CNoCliffBox::CNoCliffBox(const Vector3& SetPos, const Vector3& Max, const Vector3& Min)
{
	m_Position = SetPos;
	m_Scale = Vector3(1,1,1);
	m_Rotation = Vector3::Zero();
	m_Box = std::make_unique<CBox>(m_Position, Max, Min);
}

void CNoCliffBox::Update(float DeltaTime)
{
	CPlayer* player = GetPlayer();
	if (!player)
	{
		return;
	}

	if (!player->IsDisableCliffState() && CCollison::CollisionCheck(player->GetCapsule(), m_Box.get()))
	{
		player->SetDisableCliffState(true);
		m_IsPlayerHit = true;
	}
	else if (m_IsPlayerHit)
	{
		m_IsPlayerHit = false;
		player->SetDisableCliffState(false);
	}
}

void CNoCliffBox::Draw()
{
	if (!m_Box)
	{
		return;
	}
	m_Box->Draw();
}

void CNoCliffBox::SingleCollisonDraw()
{
	if (!m_Box)
	{
		return;
	}
	m_Box->SingleDraw();
}

CBox* CNoCliffBox::GetCollisionBox() const
{
	if (!m_Box)
	{
		return nullptr;
	}
	return m_Box.get();
}

void CNoCliffBox::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	if (m_Box)
	{
		m_Box->SetPosition(m_Position);
	}
}
