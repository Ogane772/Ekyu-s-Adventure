#pragma once
//雑魚敵の親クラス
#include "../Enemy.h"
class CBox;

class CMobEnemy : public CEnemy
{
public:
	virtual CSphere* GetRangeSphere() const override;
	virtual CBox* GetRangeBox() const override;
	bool* IsRespawn() { return &m_IsRespawn; }
	int* GetRespawnTime() { return &m_RespawnTime; }
	ItemDropData* GetItemDropData() { return&m_ItemDropData; }
	void SetItemDropData(const ItemDropData& Data) { m_ItemDropData = Data; }
	virtual void SetRangeSphere(const Vector3 local_pos, const float radius)override;
	virtual void ChangeSphereTexture(const ETextureType Type) override;
	//倒しても復活するか指定
	//引数1フラグタイプ
	//引数2復活時間
	void SetRespawn(const bool IsRespawn, const int RespawnTime);
	void ItemDrop(const EItemDropType Type, const int drop_number);
	void SetKnockBackData(const Vector3& KnockBackObjectPosition, const float KnockBackPower);

protected:
	//プレイヤーが範囲にいるか確認用Sphere
	std::unique_ptr<CSphere> m_RangeSphere;
	std::unique_ptr<CBox> m_RangeBox;
	//true時、時間で復活する
	bool m_IsRespawn = false;
	//復活時間を入れる
	int m_RespawnTime = 0;
	float m_RespawnFrameTime = 0.0f;
	float m_JumpPower = 0.0f;
	//ノックバック死ならtrue
	bool m_IsKnockBackDeath = false;
	Vector3 m_KnockBackDirection = Vector3::Zero();

	//プレイヤーが範囲内にいるか
	bool IsPlayerRange() const;

	void PlayerHitCheckEntity(const int EnemyDamage, const int PlayerDamage, const float KnockBackPower, const bool IsIgnoreFallDamage = false);
	bool UpdateKnockBackDeathMove(const float CurrentTime, const float DeltaTime, const float KnockBackTime, const float KnockBackPower, SpeedParameter& SpeedParameter);
	bool UpdateFallAttackDeathMove(const float CurrentTime, const float ScaleTime, const float ScaleY, const float DeathTime);
	void UpdateEnemyCollision();
	void RespawnSE();
};