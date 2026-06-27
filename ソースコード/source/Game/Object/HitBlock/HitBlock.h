#pragma once
#include "../../../Framework/GameObject/GameObject.h"
#include <memory>

//あるオブジェクトに当たったら壊れるブロッククラス
class CCommonObject;
class CBox;

class CHitBlock : public CGameObject
{
private:
	CCommonObject* m_Common = nullptr;
	std::unique_ptr<CBox> m_Box;
	bool m_Hit = false;
	//補間後の回転値を入れる
	Vector3 m_TargetRotation;

public:
	CHitBlock(const Vector3& SetPos, const Vector3& SetRot, const Vector3& SetScale, const Vector3& SetAddPos, const Vector3& Max, const Vector3& Min);
	virtual void Update(float DeltaTime) override;
	void SetHit(const Vector3& HitObjectFront);

	CBox* GetBox() const { return m_Box.get(); }
};
