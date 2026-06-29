#pragma once
//雑魚敵のサルクラス
#include "../MobEnemy.h"
#include "../../../../../Framework/State/FunctionStateMachine.h"
#include "../../../../../Framework/Line/Line.h"
#include "../../../../../Framework/Time/TimeScheduler.h"

class CBom;

class CMonkey : public CMobEnemy
{
private:
	enum class EMonkeyAnimationNumber
	{
		DISCOVERY_START,//プレイヤー発見
		DEATH_DIVE,
		DEATH_MOVE,
		DUMMY,
		DISCOVERY_LOOP,//プレイヤー発見時の待機アニメ
		IDLE,
		THROW,
		DUMMY1,
		DISCOVERY_END,//プレイヤーが探知範囲外に出たとき
	};
	enum class EMonkeyState
	{
		IDLE,
		DISCOVERY_START,
		DISCOVERY_LOOP,
		DISCOVERY_END,
		THROW,
		DEATH,
		RESPAWN_WAIT,
	};
	std::unordered_map<EMonkeyAnimationNumber, AnimationData> m_AnimationDataList;
	CFunctionStateMachine<EMonkeyState> m_StateMachine;
	CTimeScheduler m_ThrowTimeScheduler;
	SpeedParameter m_KnockBackSpeedParameter;

	std::unique_ptr<CCapsule> m_BodyCapsule;
	CBom* m_Bom = nullptr;

	//ステート関連
	void StartIdle();
	void UpdateIdle(float DeltaTime);
	void StartDiscoveryStart();
	void UpdateDiscoveryStart(float DeltaTime);
	void StartDiscoveryLoop();
	void UpdateDiscoveryLoop(float DeltaTime);
	void StartDiscoveryEnd();
	void UpdateDiscoveryEnd(float DeltaTime);
	void StartThrow();
	void UpdateThrow(float DeltaTime);
	void StartDeath();
	void UpdateDeath(float DeltaTime);
	void UpdateRespawnWait(float DeltaTime);

	//爆弾を持ってるときに倒されたら持っている爆弾を消す
	void DestroyNormalBom();

public:
	CMonkey(Vector3 pos);
	virtual void Update(float DeltaTime)override;
	virtual void Draw()override;
	virtual void SingleCollisonDraw();
	virtual void PlayerHitCheck()override;
	virtual void Damage(int Damage)override;
	virtual CCapsule* GetCapsule() const override;
	virtual bool IsRespawnWait() const override;
};