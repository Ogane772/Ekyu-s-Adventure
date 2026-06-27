#pragma once
#include "../PlayerStateBase.h"
#include "../../../../../Framework/State/FunctionStateMachine.h"
enum class EPlayerStateType : int;

class CPlayerStateCommonCongratulation : public CPlayerStateBase
{
public:
	CPlayerStateCommonCongratulation(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};

	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;

	virtual void Exit() override;

private:
	float m_CountCongratulationTime = 0.0f;
	bool m_IsStartEndCongratulation = false;
};
