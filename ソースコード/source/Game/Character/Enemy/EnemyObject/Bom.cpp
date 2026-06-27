#include "Bom.h"
#include "../Mob/MobEnemy.h"
#include "../../Player/Player.h"
#include "../../../Effect/EffectTypes.h"
#include "../../../../Framework/Scene/Scene.h"
#include "../../../../Framework/Scene/SceneManager.h"
#include "../../../../Framework/Sound/SoundTypes.h"
#include "../../../../Framework/Collision/Sphere.h"
#include "../../../../Framework/Collision/Collision.h"
#include "../../../../Framework/Polygon/Billboard/Billboard.h"
#include "../../../../Framework/TextureLoader/TextureTypes.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

namespace
{
	constexpr float KNOCKBACK_POWER = 0.77f;
	constexpr float MOVE_TIME = 0.8f;//移動を行う際の到達点までの時間
	constexpr float MOVE_HEIGHT = 8.0f;//移動を行う際の放物線の最高点
	constexpr float MOVE_RATE = 4.0f;//放物線計算を行う際にこの値が1だと想定している値の4分の一となるため倍率をかける
	constexpr float MOVE_NOENEMY_HIT_TIME = 0.2f;//プレイヤー位置によってはボムが動いた瞬間、投げたエネミーにボムが当たってしまうことがあるため一定時間は当たらないようにする
	constexpr float ADD_SPHERE_HEIGHT = -0.7f;
	constexpr float ADD_LINE_HEIGHT = 1.0f;
	constexpr int DAMAGE = 1;
};

void CBom::UnInit()
{
	DeleteSoundID(m_ThrowSE);
}

void CBom::DestroyBillboard()
{
 	if (m_IdleBillboard)
	{
		m_IdleBillboard.reset();
	}
}

CBom::CBom()
{
	m_StateMachine.AddState<CBom>(
		EBomState::IDLE,
		this,
		nullptr,
		nullptr,
		nullptr
	);
	m_StateMachine.AddState<CBom>(
		EBomState::MOVE,
		this,
		&CBom::StartMove,
		&CBom::UpdateMove,
		nullptr
	);
	m_StateMachine.AddState<CBom>(
		EBomState::EXPLOSION,
		this,
		&CBom::StartExplosion,
		&CBom::UpdateExplosion,
		nullptr
	);
	m_StateMachine.SetInitialState(EBomState::IDLE);

	m_ExplosionTimeScheduler.Add(0.04f, [=, this]()
	{
		m_Sphere->SetRadius(1.7f);
	});
	m_ExplosionTimeScheduler.Add(0.35f, [=, this]()
	{
		m_Sphere->SetRadius(0.0f);
	});
	m_ExplosionTimeScheduler.Add(0.48f, [=, this]()
	{
		m_IsDestroy = true;
	});
}

void CBom::Init(const Vector3 SetPos)
{
	m_Sphere = std::make_unique<CSphere>(SetPos, 0.7f);
	m_Position = SetPos;;
	m_IdleBillboard = std::make_unique<CBillboard>();
	if (m_IdleBillboard)
	{
		m_IdleBillboard->BillboardInit(ETextureType::NORMAL_BOM, m_Position, 3.0f);
	}
}

void CBom::Update(float DeltaTime)
{
	m_StateMachine.Update(DeltaTime);
}

void CBom::Draw()
{
	if (m_IdleBillboard)
	{
		m_IdleBillboard->Draw();
	}
	if (m_Sphere)
	{
		m_Sphere->Draw();
	}
}

void CBom::ChangeMoveState()
{
	m_StateMachine.ChangeState(EBomState::MOVE);
}

bool CBom::IsMoveState() const
{
	return m_StateMachine.GetState() == EBomState::MOVE;
}

void CBom::SetPosition(const Vector3& Position)
{
	m_Position = Position;
	if (!IsMoveState())
	{
		if (m_IdleBillboard)
		{
			m_IdleBillboard->UpdatePosition(m_Position);
		}
	}
	else
	{
		UpdateEffectPosition(m_MoveEffectId, m_Position);
	}
}

void CBom::StartMove()
{
	DestroyBillboard();

	m_MoveEffectId = PlayEffect(EEffectType::BOM, m_Position, 4.0f);

	CPlayer* player = GetPlayer();
	if (!player)
	{
		return;
	}
	m_ThrowSE = Play3DSoundIndex(ESoundIndex::BOM_THROW, m_Position);

	m_MoveStartPosition = m_Position;
	m_MoveEndPosition = player->GetPosition();
	//着地位置を設定。プレイヤーが空中にいることも考慮し、プレイヤーの位置から地面までラインを飛ばし、ヒット位置を終点高さとする
	m_MoveEndPosition.y += ADD_LINE_HEIGHT;
	RayCastInfo downCastInfo;
	CLine downLine;
	downLine.LineHitCheck(ECheckCollisionType::PLAYER, m_MoveEndPosition, Vector3(m_MoveEndPosition.x, 0.0f, m_MoveEndPosition.z), &downCastInfo);
	m_MoveEndPosition.y = downCastInfo.Point.y;
}

void CBom::UpdateMove(float DeltaTime)
{
	const float stateTime = m_StateMachine.GetCurrentStateTime();
	Move(stateTime);
	UpdatePlayerCollision();
	if (stateTime > MOVE_NOENEMY_HIT_TIME)
	{
		UpdateEnemyCollision();
	}
	UpdateMeshCollision();
	if (m_ThrowSE != UINT_MAX)
	{
		Update3DSoundPosition(m_ThrowSE, m_Position);
	}
}

void CBom::StartExplosion()
{
	if (m_MoveEffectId != UINT_MAX)
	{
		StopEffect(m_MoveEffectId);
	}

	PlayEffect(EEffectType::EXPLOSION, m_Position, 7.0f);
	DeleteSoundID(m_ThrowSE);
	Play3DSoundIndex(ESoundIndex::BOM_SE, m_Position);
}

void CBom::UpdateExplosion(float DeltaTime)
{
	m_ExplosionTimeScheduler.Run(DeltaTime);
	UpdatePlayerCollision();
	UpdateEnemyCollision();
}

void CBom::Move(const float CurrentTime)
{
	const float t = CurrentTime / MOVE_TIME;
	m_Position = Vector3::Lerp(m_MoveStartPosition, m_MoveEndPosition, t);

	// 放物線の高さ計算
	const float arc = MOVE_RATE * MOVE_HEIGHT * t * (1.0f - t);
	m_Position.y += arc;

	UpdateEffectPosition(m_MoveEffectId, m_Position);

	if (m_Sphere)
	{
		m_Sphere->Update(Vector3(m_Position.x, m_Position.y + ADD_SPHERE_HEIGHT, m_Position.z));
	}
}

void CBom::UpdatePlayerCollision()
{
	CPlayer* player = GetPlayer();
	if (!player)
	{
		return;
	}
	if (player->IsDeath())
	{
		return;
	}

	if (CCollison::CollisionCheck(player->GetCapsule(), m_Sphere.get()))
	{
		if (IsMoveState())
		{
			m_StateMachine.ChangeState(EBomState::EXPLOSION);
		}
		player->Damage(DAMAGE);
	}
}

void CBom::UpdateEnemyCollision()
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	std::vector<CMobEnemy*> enemys;
	scene.lock()->GetGameMobEnemyObjects(ESceneObjectType::ENEMY, enemys);

	for (CMobEnemy* enemy : enemys)
	{
		if (!enemy)
		{
			continue;
		}

		if (enemy->IsRespawnWait() || enemy->IsDeath())
		{
			continue;
		}

		bool isHit = false;
		if (enemy->GetSphere())
		{
			isHit = CCollison::CollisionCheck(m_Sphere.get(), enemy->GetSphere());
		}
		else if (enemy->GetCapsule())
		{
			isHit = CCollison::CollisionCheck(enemy->GetCapsule(), m_Sphere.get());
		}

		if (isHit)
		{
			if (IsMoveState())
			{
				m_StateMachine.ChangeState(EBomState::EXPLOSION);
			}
			enemy->SetKnockBackData(GetPosition(), KNOCKBACK_POWER);
			enemy->Damage(DAMAGE);
		}
	}
}

void CBom::UpdateMeshCollision()
{
	Vector3 normal;
	HitResult hitResult(&m_Position, &normal);
	if (CCollison::Sphere_VS_Mesh(ECheckCollisionType::OBJECT, m_Sphere.get(), hitResult))
	{
		m_StateMachine.ChangeState(EBomState::EXPLOSION);
	}
}
