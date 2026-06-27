#pragma once
#include <memory>
#include "../../../Framework/GameObject/GameObject.h"
class CStaticMesh;

class CArrow : public CGameObject
{
private:
	std::unique_ptr<CStaticMesh> m_Model;

public:
	CArrow(const Vector3& SetPos);
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
};
