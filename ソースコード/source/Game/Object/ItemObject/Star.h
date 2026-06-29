#pragma once
//プレイヤーが拾うスタークラス
#include <vector>
#include "../../../Framework/GameObject/GameObject.h"
#include "../../../Framework/State/FunctionStateMachine.h"

class CStaticMesh;
class CSphere;

class CStar : public CGameObject
{
private:
	enum class EStarState
	{
		NORMAL,
		EMERGENCE,
	};
	//スター出現時のパラメータ
	struct EmergenceParameter
	{
		float BounceTime = 0.0f;//跳ねる時間
		float BounceHeight = 0.0f;//跳ねる高さ
		EmergenceParameter() {};
		EmergenceParameter(const float InBounceTime, const float InBounceHeight)
			: BounceTime(InBounceTime), BounceHeight(InBounceHeight) {};
	};
	CFunctionStateMachine<EStarState> m_StateMachine;
	std::vector<EmergenceParameter> m_EmergenceParameterList;//スター出現時の跳ねる回数分要素を確保する
	float m_DefaultBounceHeight = 0.0f;
	int m_BounceCount = 0;
	std::unique_ptr<CStaticMesh> m_Model;
	std::unique_ptr<CSphere> m_Sphere;

	//演出用フラグ
	bool m_IsStarSceneChange = false;
	bool m_IsDisableShadow = false;

	void UpdateNormal(float DeltaTime);
	void StartEmergence();
	void UpdateEmergence(float DeltaTime);

public:
	CStar(const Vector3 SetPos);
	virtual void Update(float DeltaTime) override;
	virtual void UnInit() override;
	virtual void Draw() override;
	virtual void ShadowDraw() override;

	virtual bool* IsDisableShadow() override { return &m_IsDisableShadow; }
	void SetDisableShadow(const bool Disable) { m_IsDisableShadow = Disable; }
	
	void SetEmergenceState();
	void EnableStarSceneChange() { m_IsStarSceneChange = true; }
	void InitBodyCollisionCheck() 
		;
	virtual CSphere* GetSphere() const override;
	virtual MATERIAL* GetMaterial() const override;
	virtual void SetPosition(const Vector3& SetPos)override;
	virtual bool* IsEnableLight()override;
	virtual void SetEnableLight(const bool Enable)override;
};
