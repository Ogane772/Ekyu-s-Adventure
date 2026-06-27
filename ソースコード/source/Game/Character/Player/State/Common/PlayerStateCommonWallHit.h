#pragma once
#include "../PlayerStateBase.h"
#include "../../../../../Framework/State/FunctionStateMachine.h"
enum class EPlayerStateType : int;

class CPlayerStateCommonWallHit : public CPlayerStateBase
{
public:
	CPlayerStateCommonWallHit(CPlayer* InOwner);

	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;

private:
	enum class EWallHitState
	{
		NONE,
		START,
		SIT,
		END,
	};
	CFunctionStateMachine<EWallHitState> m_StateMachine;
	EPlayerStateType m_ReturnState;

	void WallHitStart();
	void WallHitStartUpdate(float DeltaTime);
	void WallHitSitStart();
	void WallHitSitUpdate(float DeltaTime);
	void WallHitEndStart();
	void WallHitEndUpdate(float DeltaTime);
};
