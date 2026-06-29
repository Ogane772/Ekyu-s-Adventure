#pragma once
//プレイヤーが触れた際カメラに対して高さ制限を行うボックス
#include "../GameObject/GameObject.h"
#include <memory>

class CBox;

class CCameraHeightLockBox : public CGameObject
{
private:
	std::unique_ptr<CBox> m_Box;
	float m_HeightLockPosition = 0.0f;//カメラの高さを固定する位置
	bool m_IsPlayerHit = false;

public:
	CCameraHeightLockBox(const Vector3& SetPos, const Vector3& Max, const Vector3& Min);
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	virtual void SingleCollisonDraw() override;
	virtual CBox* GetCollisionBox() const override;
	virtual void SetPosition(const Vector3& SetPos)override;
	float* GetHeightLockPosition() { return &m_HeightLockPosition; }
	void SetHeightLockPosition(const float Height) { m_HeightLockPosition = Height; }
};
