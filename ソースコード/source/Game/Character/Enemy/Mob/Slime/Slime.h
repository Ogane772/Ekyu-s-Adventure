#pragma once
//雑魚敵のスライムクラス
#include "../MobEnemy.h"
#include <memory>
#include "../../../../../Framework/State/FunctionStateMachine.h"
class CSphere;
class CCapsule;
class CBox;

class CSlime : public CMobEnemy
{
private:
	enum class ESlimeAnimationNumber
	{
		DISCOVERY,
		DUMMY,
		DUMMY2,
		DEATH,
		IDLE,
		DUMMY3,
		WALK,
	};
	enum class ESlimeState
	{
		IDLE,
		WALK,
		PLAYER_MOVE,
		DISCOVERY,
		DEATH,
		RESPAWN_WAIT,
	};
	enum class ESlimeSpeedParameterType
	{
		WALK,
		PLAYER_MOVE,
		KNOCKBACK_DEATH
	};

	//アニメーション
	std::unordered_map<ESlimeAnimationNumber, AnimationData> m_AnimationDataList;
	CFunctionStateMachine<CSlime::ESlimeState> m_StateMachine;
	std::unordered_map<ESlimeSpeedParameterType, SpeedParameter> m_SpeedParameterList;

	//プレイヤーが探知範囲を往復した際に延々と発見アニメーションを繰り返さないようにする
	//一度待機状態になるまでは発見アニメーションをスキップする
	bool m_IsPlayerDiscovery = false;

	std::unique_ptr<CSphere> m_BodySphere;
	UINT m_WalkSE = UINT_MAX;
	UINT m_DashSE = UINT_MAX;

	//ステート関連
	void StartIdle();
	void UpdateIdle(float DeltaTime);
	void StartWalk();
	void UpdateWalk(float DeltaTime);
	void StartPlayerMove();
	void UpdatePlayerMove(float DeltaTime);
	void StartDiscovery();
	void UpdateDiscovery(float DeltaTime);
	void StartDeath();
	void UpdateDeath(float DeltaTime);
	void UpdateRespawnWait(float DeltaTime);

	//他のエネミーと当たったら押し出す。地形チェック
	void BodyCollisionCheck(const float DeltaTime);

public:
	CSlime(Vector3 pos);
	virtual void Update(float DeltaTime)override;
	virtual void Draw()override;
	virtual void SingleCollisonDraw();
	virtual void PlayerHitCheck()override;
	virtual void Damage(const int Damage)override;

	virtual CSphere* GetSphere() const override { return m_BodySphere.get(); }
	virtual void SetBodySphere(const Vector3 local_pos, const float radius)override;
	virtual bool IsRespawnWait() const override;
};