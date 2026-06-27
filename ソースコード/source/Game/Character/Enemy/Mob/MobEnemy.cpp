#include "MobEnemy.h"
#include <random>
#include <chrono>
#include "../../Player/Player.h"
#include "../../../Object/ItemObject/Star.h"
#include "../../../Object/ItemObject/Acorn.h"
#include "../../../Object/ItemObject/Heart.h"
#include "../../../../Framework/Time/Time.h"
#include "../../../../Framework/Sound/SoundTypes.h"
#include "../../../../Framework/Scene/SceneManager.h"
#include "../../../../Framework/Scene/Scene.h"
#include "../../../../Framework/Collision/Sphere.h"
#include "../../../../Framework/Collision/Box.h"
#include "../../../../Framework/Collision/Collision.h"

CSphere* CMobEnemy::GetRangeSphere() const
{
	return m_RangeSphere.get();
}

CBox* CMobEnemy::GetRangeBox() const
{
	return m_RangeBox.get();
}

void CMobEnemy::SetRangeSphere(const Vector3 local_pos, const float radius)
{
	if (m_RangeSphere)
	{
		m_RangeSphere->SetAddSpherePosition(local_pos);
		m_RangeSphere->SetRadius(radius);
		m_RangeSphere->Update(m_FirstPosition);
	}
}

void CMobEnemy::ChangeSphereTexture(const ETextureType Type)
{
	if (m_RangeSphere)
	{
		m_RangeSphere->ChangeSphereTexture(Type);
	}
}

void CMobEnemy::SetRespawn(const bool IsRespawn, const int RespawnTime)
{
	CTime* time = CTime::GetInstance();
	if (!time)
	{
		return;
	}
	m_IsRespawn = IsRespawn;
	m_RespawnTime = RespawnTime;
	m_RespawnFrameTime = static_cast<float>(m_RespawnTime) * static_cast<float>(time->GetFrameTime());
}

void CMobEnemy::ItemDrop(const EItemDropType Type, const int drop_number)
{
	if (Type == EItemDropType::NO_ITEM)
	{
		return;
	}
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	switch (Type)
	{
	case EItemDropType::ACORN:
	{
		for (int i = 0; i < drop_number; i++)
		{
			CAcorn* acorn = new CAcorn(Vector3(m_Position.x, m_Position.y + 3.0f, m_Position.z));
			acorn->InitBodyCollisionCheck();
			acorn->SetOriginalNumber(-1);
			scene.lock()->AddGameObject<CAcorn>(ESceneObjectType::BILLBOARD, acorn);
		}
		PlaySoundIndex(ESoundIndex::ItemDropData);
		break;
	}
	case EItemDropType::HEART:
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> random(1, 100);
		const int drop = random(mt);
		if (drop <= drop_number)
		{
			if (CHeart* heart = new CHeart(Vector3(m_Position.x, m_Position.y, m_Position.z), true))
			{
				heart->SetOriginalNumber(-1);
				scene.lock()->AddGameObject<CHeart>(ESceneObjectType::NOMESH_OBJECT, heart);
				PlaySoundIndex(ESoundIndex::ItemDropData);
			}
		}
		break;
	}
	case EItemDropType::STAR:
	{
		if(CStar* star = new CStar(Vector3(m_Position.x, m_Position.y + 6.0f, m_Position.z)))
		{
			star->SetEmergenceState();
			star->SetOriginalNumber(-1);
			scene.lock()->AddGameObject<CStar>(ESceneObjectType::NOMESH_OBJECT, star);
		}
		break;
	}
	}

}

void CMobEnemy::PlayerHitCheckEntity(const int EnemyDamage, const int PlayerDamage, const float KnockBackPower, const bool IsIgnoreFallDamage /*= false*/)
{
	if (!m_Player)
	{
		return;
	}
	if (IsDeath())
	{
		return;
	}

	if (CCollison::CollisionCheck(m_Player->GetAttackSphere(), GetSphere()) ||
		CCollison::CollisionCheck(GetCapsule(), m_Player->GetAttackSphere()))
	{
		if (m_Player->IsEnemyKnockBackDamageState())
		{
			if (m_Player->IsTailAttackState())
			{
				TailAttackDeath();
			}
			else
			{
				DiveAttackDeath();
			}
			SetKnockBackData(m_Player->GetPosition(), KnockBackPower);
			Damage(EnemyDamage);
			return;
		}
		else if (!IsIgnoreFallDamage && m_Player->IsEnemyFallDamageState())
		{
			PlaySoundIndex(ESoundIndex::JUMP_HIT);
			if (m_Player->IsEnemyFallJumpState())
			{
				m_Player->ChangeNormalJumpState(ENormalJumpState::NO_JUMP);
			}
			Damage(EnemyDamage);
			return;
		}
	}

	if (CCollison::CollisionCheck(m_Player->GetCapsule(), GetSphere()) ||
		CCollison::CollisionCheck(m_Player->GetCapsule(), GetCapsule()))
	{
		m_Player->Damage(PlayerDamage);
	}
}

bool CMobEnemy::IsPlayerRange() const
{
	if (!m_RangeSphere)
	{
		return false;
	}

	if (!m_Player)
	{
		return false;
	}

	if (m_Player->IsDeath())
	{
		return false;
	}

	bool isHitCollision = false;
	if (m_RangeSphere->GetRadius() > 0.0f)
	{
		isHitCollision = CCollison::CollisionCheck(m_Player->GetCapsule(), m_RangeSphere.get());
	}
	else
	{
		isHitCollision = CCollison::CollisionCheck(m_Player->GetCapsule(), m_RangeBox.get());
	}
	return isHitCollision;
}

bool CMobEnemy::UpdateKnockBackDeathMove(const float CurrentTime, const float DeltaTime, const float KnockBackTime, const float KnockBackPower, SpeedParameter& SpeedParameter)
{
	if (CurrentTime < KnockBackTime)
	{
		m_Position.y += m_JumpPower * GetTimeScale();
		m_JumpPower -= KnockBackPower * DeltaTime;
		SpeedParameter.MoveDirection = m_KnockBackDirection;
		Move(SpeedParameter, DeltaTime);
		return false;
	}
	return true;
}

bool CMobEnemy::UpdateFallAttackDeathMove(const float CurrentTime, const float ScaleTime, const float ScaleY, const float DeathTime)
{
	const float currentTime = (std::min)(CurrentTime, ScaleTime);//rateを0-1にするため現在の経過時間がスケール終了時間を超えないようにする
	const float rate = currentTime / ScaleTime;
	m_Scale.y = std::lerp(m_FirstScale.y, ScaleY, rate);
	if (CurrentTime > DeathTime)
	{
		return true;
	}
	return false;
}

void CMobEnemy::UpdateEnemyCollision()
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	std::vector<CEnemy*> enemys;
	scene.lock()->GetGameEnemyObjects(ESceneObjectType::ENEMY, enemys);

	for (CEnemy* enemy : enemys)
	{
		if (!enemy)
		{
			continue;
		}

		if (enemy->IsRespawnWait())
		{
			continue;
		}
		//エネミーなどの体に当たったら押し出す
		if (m_Id == enemy->GetId())
		{
			continue;
		}
		float saveY = m_Position.y;
		bool hit = false;
		if (CSphere* sphere = enemy->GetSphere())
		{
			hit = CCollison::CollisionCheck(GetSphere(), sphere, &m_Position);
		}
		else if (CCapsule* capsule = enemy->GetCapsule())
		{
			hit = CCollison::CollisionCheck(capsule, GetSphere(), &m_Position);
		}
		if (hit)
		{
			m_Position.y = saveY;
		}
	}
}

void CMobEnemy::RespawnSE()
{
	Play3DSoundIndex(ESoundIndex::RESPAWN, m_Position);
}

void CMobEnemy::SetKnockBackData(const Vector3& KnockBackObjectPosition, const float KnockBackPower)
{
	m_IsKnockBackDeath = true;
	m_KnockBackDirection = m_Position - KnockBackObjectPosition;
	m_KnockBackDirection.Normalize();
	m_JumpPower = KnockBackPower;
}
