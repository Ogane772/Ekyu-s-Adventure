#pragma once
#include "../PlayerStateBase.h"
enum class EPlayerStateType : int;

class CPlayerStateLandTailAttack : public CPlayerStateBase
{
public:
	CPlayerStateLandTailAttack(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};
	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;

	virtual void Exit() override;

private:
	float m_CountTalkAttackTime = 0.0f;
};