#pragma once
#include "../../../../Framework/GameObject/GameObject.h"
#include "../../../../Framework/State/FunctionStateMachine.h"
#include "../../../../Framework/Time/TimeScheduler.h"

class CSphere;
class CBillboard;

class CBom : public CGameObject
{
private:
	enum class EBomState
	{
		IDLE,
		MOVE,
		EXPLOSION,
	};
	CFunctionStateMachine<EBomState> m_StateMachine;
	CTimeScheduler m_ExplosionTimeScheduler;
	Vector3 m_MoveStartPosition;
	Vector3 m_MoveEndPosition;
	//ビルボードは静止状態のボム。エフェクトは移動中のボムを表現
	std::unique_ptr<CBillboard> m_IdleBillboard;
	UINT m_MoveEffectId = UINT_MAX;
	std::unique_ptr<CSphere> m_Sphere;
	UINT m_ThrowSE = UINT_MAX;

	//ステート関連
	void StartMove();
	void UpdateMove(float DeltaTime);
	void Move(const float CurrentTime);
	void StartExplosion();
	void UpdateExplosion(float DeltaTime);
	void UpdatePlayerCollision();
	void UpdateEnemyCollision();
	void UpdateMeshCollision();

public:
	CBom();
	virtual void Init() override {};
	virtual void Init(const Vector3 SetPos);
	virtual void Update(float DeltaTime) override;
	virtual void UnInit() override;
	void DestroyBillboard();
	virtual void Draw() override;
	void ChangeMoveState();
	bool IsMoveState() const;
	virtual void SetPosition(const Vector3& Position) override;
};
