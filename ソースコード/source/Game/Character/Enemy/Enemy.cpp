#include <stdio.h>
#include "Enemy.h"
#include "../Enemy/Mob/Slime/Slime.h"
#include "../Enemy/Mob/Monkey/Monkey.h"
#include "../Enemy/Mob/ThornMan/ThornMan.h"
#include "../Enemy/Boss/Gorilla.h"
#include "../../Effect/EffectTypes.h"
#include "../../Character/Player/Player.h"
#include "../../../Framework/Scene/SceneManager.h"
#include "../../../Framework/Scene/Scene.h"
#include "../../../Framework/Polygon/Billboard/Billboard.h"
#include "../../../Framework/Sound/SoundTypes.h"
#include "../../../Framework/Time/Time.h"
#include "../../../Framework/HitStop/HitStop.h"
#include "../../../Framework/Collision/Collision.h"
#include "../../../Framework/Collision/CollisionTag.h"
#include "../../../Framework/TextureLoader/TextureTypes.h"
#include "../../../Framework/Shadow/ShadowType.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

namespace
{
	constexpr float TARGET_GOAL_LENGTH = 0.7f;
	constexpr float ADD_LINE_HEIGHT = 1.0f;
	constexpr float LINE_HEIGHT_SPEED = 18.75f;
	constexpr float ADD_LINE_START_HEIGHT = 1.5f;
	constexpr float ADD_LINE_END_HEIGHT = -1.0f;
};

// vectorからindex番目の要素を削除する
template<typename T>
void remove(std::vector<T>& vector, unsigned int index)
{
	vector.erase(vector.begin() + index);
}

UINT CEnemy::GenerateID()
{
	static UINT counter = 0;
	return counter++;
}

CEnemy::~CEnemy()
{
	MapObjectNull(m_OriginalNumber);
}

void CEnemy::ShadowDraw()
{
	if (!m_SkeletalMesh)
	{
		return;
	}
	if (IsRespawnWait())
	{
		return;
	}
	if (!m_IsCulling)
	{
		return;
	}
	if (m_IsEditorDisableShadow)
	{
		return;
	}
	m_SkeletalMesh->ShadowDraw(EShadowType::DOWN_SHADOW, m_World);
}

CEnemy* CEnemy::Create(const EEnemyType Type, const Vector3& Position)
{
	switch (Type)
	{
	case EEnemyType::SLIME:
		return new CSlime(Position);
	case EEnemyType::MONKEY:
		return new CMonkey(Position);
	case EEnemyType::THORN_MAN:
		return new CThornMan(Position);
	case EEnemyType::GORILLA:
		return new CGorilla(Position);
	}
	return nullptr;
}

CEnemy::CEnemy()
{
	UniInitTargetPointImage();
	m_Id = GenerateID();
	m_Player = GetPlayer();
}

void CEnemy::TargetPointDraw()
{
	for(std::unique_ptr<CBillboard>& targetPoint : m_TargetPointImage)
	{
		if (targetPoint)
		{
			targetPoint->Draw();
		}
	}
}

void CEnemy::AddTargetPosition(const Vector3& AddPos)
{
	m_TargetPoint.emplace_back(Vector3(m_FirstPosition + AddPos));
	if (std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>())
	{
		billboard->BillboardInit(ETextureType::TARGET, m_FirstPosition + AddPos, 2.5f);
		m_TargetPointImage.emplace_back(std::move(billboard));
	}
	m_MaxTargetPoint++;
}

void CEnemy::DeleteTargetPosition(const int index)
{
	if (m_TargetPointImage[index])
	{
		m_TargetPointImage[index].release();
	}
	remove(m_TargetPointImage, index);
	remove(m_TargetPoint, index);
	m_MaxTargetPoint--;
}

void CEnemy::SetTargetPointImagePosition(const int index, const Vector3& SetPos)
{
	if (m_TargetPointImage[index])
	{
		m_TargetPointImage[index]->UpdatePosition(SetPos);
	}
}

void CEnemy::UniInitTargetPointImage()
{
	for (std::unique_ptr<CBillboard>& targetPoint : m_TargetPointImage)
	{
		if (targetPoint)
		{
			targetPoint.reset();
		}
	}
}

void CEnemy::TailAttackDeath()
{
	PlaySoundIndex(ESoundIndex::TAIL_HIT);
}

void CEnemy::DiveAttackDeath()
{
	PlaySoundIndex(ESoundIndex::DIVE_HIT);
}

void CEnemy::SetTargetPoint(int num, ...)
{
	va_list arg;

	va_start(arg, num);

	// 引数の数だけ繰り返し
	int cnt;
	for (cnt = 0; cnt < num; cnt++)
	{
		m_TargetPoint.emplace_back(va_arg(arg, Vector3));
	}

	va_end(arg);
	//エネミーの初期位置に代入
	for (cnt = 0; cnt < num; cnt++)
	{
		m_TargetPoint[cnt] += m_FirstPosition;
		if (std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>())
		{
			billboard->BillboardInit(ETextureType::TARGET, m_TargetPoint[cnt], 2.5f);
			m_TargetPointImage.emplace_back(std::move(billboard));
		}
	}
	//配列なので最大数をマイナス-1する
	m_MaxTargetPoint = num - 1;
}

void CEnemy::CreateHitEffect(const Vector3 pos, const float scale)
{
	PlayEffect(EEffectType::ATTACK_HIT2, pos, scale);
}

void CEnemy::CreateDeathEffect(const Vector3 pos, const float scale)
{
	PlayEffect(EEffectType::REVIVAL, pos, scale);
	PlaySoundIndex(ESoundIndex::ENEMY_DEATH);
}

void CEnemy::TowardThePlayer(const float RotationSpeed, float DeltaTime)
{
	if(!m_Player)
	{
		return;
	}
	Vector3 direction = m_Player->GetPosition() - m_Position;
	direction.y = 0.0f;

	DirectionRotation(RotationSpeed, DeltaTime, direction);
	UpdateVector();
}

bool CEnemy::IsTargetGoal(const Vector3& Direction) const
{
	return fabs(Direction.LengthSq()) < TARGET_GOAL_LENGTH;
}

bool CEnemy::PlayerMove(SpeedParameter& SpeedParam, const float DeltaTime, const bool IsForcedBrake /*= false*/)
{
	if (!m_Player)
	{
		return false;
	}
	Vector3 direction = m_Player->GetPosition() - m_Position;
	direction.y = 0.0f;
	SpeedParam.MoveDirection = direction.Normalized();
	const bool isTargetGoal = IsTargetGoal(direction);
	if (!isTargetGoal)
	{
		Move(SpeedParam, DeltaTime);
	}
	return isTargetGoal;
}

bool CEnemy::UpdateMeshVsBodyCollision()
{
	if (!m_IsBodySphereCollisionCheck)
	{
		return false;
	}
	bool isHit = false;
	CSphere* sphere = GetSphere();
	CCapsule* capsule = GetCapsule();
	
	const float saveY = m_Position.y;
	Vector3 normal;
	HitResult hitResult(&m_Position, &normal);
	if (sphere)
	{
		isHit = CCollison::Sphere_VS_Mesh(ECheckCollisionType::OBJECT, sphere, hitResult);
	}
	else
	{
		isHit = CCollison::Capsule_VS_Mesh(ECheckCollisionType::OBJECT, capsule, hitResult);
	}
	m_Position.y = saveY;
	return isHit;
}

bool CEnemy::UpdateGroundCollision(const float DeltaTime, RayCastInfo& OutRayCastInfo, const bool IsNoLineHitPositionReset /*= true*/)
{
	if (!m_IsDownLineCollisionCheck)
	{
		return false;
	}
	CLine line;
	//地面との判定
	const bool isDownLineHit = line.LineHitCheck(ECheckCollisionType::OBJECT, Vector3(m_Position.x, m_Position.y + ADD_LINE_START_HEIGHT, m_Position.z), Vector3(m_Position.x, m_Position.y + ADD_LINE_END_HEIGHT, m_Position.z), &OutRayCastInfo);
	if (isDownLineHit)
	{
		const float hitPoint = OutRayCastInfo.Point.y;
		if (hitPoint + ADD_LINE_HEIGHT >= m_Position.y)
		{
			m_Position.y = std::lerp(m_Position.y, hitPoint, LINE_HEIGHT_SPEED * DeltaTime);
		}
	}
	else if (IsNoLineHitPositionReset)
	{
		//空中に浮いている状態なのでこのフレームの初期位置に戻す
		m_Position = m_PrevPosition;
	}
	return isDownLineHit;
}

bool CEnemy::UpdateGroundCollision(const float DeltaTime)
{
	RayCastInfo rayCastInfo;
	return UpdateGroundCollision(DeltaTime, rayCastInfo);
}