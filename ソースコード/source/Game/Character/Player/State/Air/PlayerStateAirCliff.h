#pragma once
#include "../PlayerStateBase.h"
#include "../../../../../Framework/State/FunctionStateMachine.h"
enum class EPlayerStateType : int;

class CPlayerStateAirCliff : public CPlayerStateBase
{
public:
	CPlayerStateAirCliff(CPlayer* InOwner);

	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;

	virtual void Exit() override;

private:
	enum class ECliffState
	{
		NONE,
		START,
		LOOP,
		UP,
	};
	CFunctionStateMachine<ECliffState> m_StateMachine;
	EPlayerStateType m_ReturnState;

	void CliffStart();
	void CliffStartUpdate(float DeltaTime);
	void CliffLoopStart();
	void CliffLoopUpdate(float DeltaTime);
	void CliffUpStart();
	void CliffUpUpdate(float DeltaTime);
	void CliffUpEnd();
};
