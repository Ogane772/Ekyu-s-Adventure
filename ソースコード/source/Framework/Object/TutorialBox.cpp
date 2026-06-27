#include "TutorialBox.h"
#include "../Collision/Box.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Collision/Collision.h"
#include "../../Game/Character/Player/Player.h"
#include "../../Game/UserInterface/MainGameUI.h"

CTutorialBox::CTutorialBox(const Vector3& SetPos, const Vector3& Max, const Vector3& Min)
{
	m_Position = SetPos;
	m_Scale = Vector3(1, 1, 1);
	m_Rotation = Vector3::Zero();
	m_Box = std::make_unique<CBox>(m_Position, Max, Min);
}

void CTutorialBox::Update(float DeltaTime)
{
	CPlayer* player = GetPlayer();

	if (CCollison::CollisionCheck(player->GetCapsule(), m_Box.get()))
	{
		std::weak_ptr<CScene> scene = GetScene();
		if (scene.expired())
		{
			return;
		}
		if (CMainGameUI* ui = scene.lock()->GetGameObject<CMainGameUI>(ESceneObjectType::UI))
		{
			ui->SetTutorialNumber(m_TutorialNumber);
		}
	}
}

void CTutorialBox::Draw()
{
	if (!m_Box)
	{
		return;
	}
	m_Box->Draw();
}

void CTutorialBox::SingleCollisonDraw()
{
	if (!m_Box)
	{
		return;
	}
	m_Box->SingleDraw();
}

CBox* CTutorialBox::GetCollisionBox() const
{
	if (!m_Box)
	{
		return nullptr;
	}
	return m_Box.get();
}

void CTutorialBox::SetPosition(const Vector3& SetPos)
{
	m_Position = SetPos;
	if (m_Box)
	{
		m_Box->SetPosition(m_Position);
	}
}
