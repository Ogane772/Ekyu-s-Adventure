#pragma once
//配置時にプレイヤーのスタート位置にするクラス
#include "../../../Framework/GameObject/GameObject.h"
class CStaticMesh;
class CSphere;

class CPlayerStart : public CGameObject
{
private:
	std::unique_ptr<CStaticMesh> m_Model;
	std::unique_ptr<CSphere> m_Sphere;

public:
	CPlayerStart(const Vector3& SetPos, const Vector3& SetRot);
	void Update(float DeltaTime) override;
	void Draw() override;
	CSphere* GetSphere() const  override { return m_Sphere.get(); }
	void SetPosition(const Vector3& SetPos) override;
};
