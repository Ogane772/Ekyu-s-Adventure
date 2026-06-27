#pragma once
#include "../PlayerStateBase.h"
#include "../../../../../Framework/State/FunctionStateMachine.h"
enum class EPlayerStateType : int;

class CPlayerStateCommonDamage : public CPlayerStateBase
{
public:
	CPlayerStateCommonDamage(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};

	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;
};
