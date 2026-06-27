#pragma once
#include "../Enemy.h"
#include "../../../../Framework/State/FunctionStateMachine.h"
#include "../../../../Framework/Time/TimeScheduler.h"
#include <unordered_map>
class CArrow;
class CCapsule;

class CGorilla : public CEnemy
{
private:
	enum class EGorillaAnimationNumber
	{
		IDLE,
		DRUMMING,
		DEFAULT,
		DEATH,
		ATTACK_START,
		ATTACK_LOOP,
		ATTACK_HITEND,
		ATTACK_HITDAMAGE,
		ATTACK_HIT_START,
		ATTACK_HIT_LOOP,
		ATTACK_END,
		ATTACK_JUMP_START,
		ATTACK_JUMP_LOOP,
		ATTACK_JUMP_END,
		JUMP_GUARD,
	};
	enum class EGorillaState
	{
		DRUMMING,
		IDLE,
		PLAYER_MOVE,
		JUMP_ATTACK,
		STUN,
		DAMAGE,
		DEATH,
	};
	enum class EJumpAttackState
	{
		START,
		LOOP,
		END,
		WAIT,
	};
	enum class EGorillaSpeedParameterType
	{
		PLAYER_MOVE,
		FIRST_DAMAGE_PLAYER_MOVE,
		SECOND_DAMAGE_PLAYER_MOVE,
		JUMP_ATTACK,
		FIRST_DAMAGE_JUMP_ATTACK,
		SECOND_DAMAGE_JUMP_ATTACK,
	};

	//アニメーション
	std::unordered_map<EGorillaAnimationNumber, AnimationData> m_AnimationDataList;
	CFunctionStateMachine<CGorilla::EGorillaState> m_StateMachine;
	CFunctionStateMachine<CGorilla::EJumpAttackState> m_JumpAttackStateMachine;
	std::unordered_map<EGorillaSpeedParameterType, SpeedParameter> m_SpeedParameterList;
	CTimeScheduler m_DrummingTimeScheduler;
	CTimeScheduler m_DamageTimeScheduler;
	CTimeScheduler m_DeathTimeScheduler;
	//現在の体力をキーとしたジャンプ攻撃を行う回数を格納するマップ
	std::unordered_map<int, int> m_LifeJumpAttackList;
	//現在の体力をキーとした速度パラメータを格納するマップ
	std::unordered_map<int, SpeedParameter> m_LifePlayerMoveSpeedList;
	std::unordered_map<int, SpeedParameter> m_LifeJumpAttackSpeedList;

	std::unique_ptr<CCapsule> m_BodyCapsule;
	std::unique_ptr<CSphere> m_AttackSphere;
	UINT m_StunEffectId = UINT_MAX;
	UINT m_SwingEffectId = UINT_MAX;
	CArrow* m_Arrow = nullptr;
	int m_JumpAttackCount = 0;

	//ステート関連
	void StartDrumming();
	void UpdateDrumming(float DeltaTime);
	void StartIdle();
	void UpdateIdle(float DeltaTime);
	void StartPlayerMove();
	void UpdatePlayerMove(float DeltaTime);
	void StartJumpAttack();
	void UpdateJumpAttack(float DeltaTime);
	void StartStun();
	void UpdateStun(float DeltaTime);
	void StartDamage();
	void UpdateDamage(float DeltaTime);
	void StartDeath();
	void UpdateDeath(float DeltaTime);

	//ジャンプ攻撃のステート関連
	void StartJumpAttackStart();
	void UpdateJumpAttackStart(float DeltaTime);
	void StartJumpAttackLoop();
	void UpdateJumpAttackLoop(float DeltaTime);
	void StartJumpAttackEnd();
	void UpdateJumpAttackEnd(float DeltaTime);
	void UpdateJumpAttackWait(float DeltaTime);

	void BodyCollisionCheck();
	void ChoiceNextAttackState();

public:
	CGorilla(Vector3 pos);
	virtual void Update(float DeltaTime)override;
	virtual void Draw()override;;
	virtual void PlayerHitCheck()override;
	virtual void Damage(int damage)override;;
	virtual CCapsule* GetCapsule() const override { return m_BodyCapsule.get(); }
};