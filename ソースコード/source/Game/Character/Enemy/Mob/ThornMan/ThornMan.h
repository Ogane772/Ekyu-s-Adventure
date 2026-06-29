#pragma once
//雑魚敵のトゲマンクラス
#include "../MobEnemy.h"
#include "../../../../../Framework/State/FunctionStateMachine.h"

class CThornMan : public CMobEnemy
{
private:
	enum class EThornManAnimationNumber
	{
		PLAYER_MOVE,
		DEATH,
		DEFAULT,
		IDLE,
		DISCOVERY,
		WALK,
	};
	enum class EThornManState
	{
		IDLE,
		WALK,
		PLAYER_MOVE,
		DISCOVERY,
		DEATH,
		RESPAWN_WAIT,
	};
	enum class EThornManSpeedParameterType
	{
		WALK,
		PLAYER_MOVE,
	};

	//アニメーション
	std::unordered_map<EThornManAnimationNumber, AnimationData> m_AnimationDataList;
	CFunctionStateMachine<CThornMan::EThornManState> m_StateMachine;
	std::unordered_map<EThornManSpeedParameterType, SpeedParameter> m_SpeedParameterList;

	//プレイヤーが範囲にいるか確認用Sphere
	std::unique_ptr<CCapsule> m_RangeCapsule;
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
;
	//他のエネミーと当たったら押し出す
	void BodyCollisionCheck(const float DeltaTime);
public:
	CThornMan(Vector3 pos);
	virtual void Update(float DeltaTime)override;
	virtual void Draw()override;
	virtual void SingleCollisonDraw();
	virtual void PlayerHitCheck()override;
	virtual void Damage(int Damage)override;

	virtual void SetBodySphere(const Vector3 local_pos, const float radius)override;
	virtual CSphere* GetSphere() const override { return m_BodySphere.get(); }
	virtual bool IsRespawnWait() const override;
};