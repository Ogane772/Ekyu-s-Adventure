#pragma once
#include "../PlayerStateBase.h"
#include "../../../../../Framework/State/FunctionStateMachine.h"
enum class EPlayerStateType : int;

class CPlayerStateAirHipDrop : public CPlayerStateBase
{
public:
	CPlayerStateAirHipDrop(CPlayer* InOwner);

	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;

private:
	enum class EHipDropState
	{
		NONE,
		START,
		END
	};
	CFunctionStateMachine<EHipDropState> m_StateMachine;
	EPlayerStateType m_ReturnState;

	void HipDropStart();
	void HipDropStartUpdate(float DeltaTime);
	void HipDropEndStart();
	void HipDropEndUpdate(float DeltaTime);
};
