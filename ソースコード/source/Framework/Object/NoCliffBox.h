#pragma once
//プレイヤーが触れている時、崖掴みをできなくするボックス
#include "../GameObject/GameObject.h"
#include <memory>

class CBox;
class CNoCliffBox : public CGameObject
{
private:
	std::unique_ptr<CBox> m_Box;
	bool m_IsPlayerHit = false;

public:
	CNoCliffBox(const Vector3& SetPos, const Vector3& Max, const Vector3& Min);
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	virtual void SingleCollisonDraw() override;
	virtual CBox* GetCollisionBox() const override;
	virtual void SetPosition(const Vector3& SetPos)override;
};
