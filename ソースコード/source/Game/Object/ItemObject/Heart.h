#pragma once
//プレイヤーが拾うハートクラス
#include "../../../Framework/GameObject/GameObject.h"
#include "../../../Framework/State/FunctionStateMachine.h"

class CStaticMesh;
class CSphere;
class CLine;

class CHeart : public CGameObject
{
private:
	enum class EHeartState
	{
		NORMAL,
		DROP,
	};
	CFunctionStateMachine<EHeartState> m_StateMachine;
	std::unique_ptr<CStaticMesh> m_Model;
	std::unique_ptr<CSphere> m_Sphere;
	std::unique_ptr<CLine> m_DownLine;

	bool m_IsNoPlayerHit = false;
	bool m_IsDisableShadow = false;

	void StartDrop();
	void UpdateDrop(float DeltaTime);
	bool IsInvincible() const;

public:
	CHeart(const Vector3& SetPos, const bool IsDrop);
	virtual void Update(float DeltaTime)override;
	virtual void UnInit()override;
	virtual void Draw()override;
	virtual void ShadowDraw() override;

	void InitBodyCollisionCheck();
	virtual bool* IsDisableShadow()override { return &m_IsDisableShadow; }
	void SetDisableShadow(const bool Disable) { m_IsDisableShadow = Disable; }
	virtual CSphere* GetSphere() const override;
	virtual MATERIAL* GetMaterial() const override;
	virtual void SetPosition(const Vector3& SetPos)override;
	virtual bool* IsEnableLight()override;
	virtual void SetEnableLight(const bool Enable)override;
	
};
