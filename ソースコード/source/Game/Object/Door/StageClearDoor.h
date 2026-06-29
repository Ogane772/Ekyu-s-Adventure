#pragma once
//ステージクリア時にインタラクトするドア
#include "../../../Framework/GameObject/GameObject.h"
#include <vector>
#include <memory>
class CGameObject;
class CBox;
class CStaticMesh;

class CStageClearDoor : public CGameObject
{
private:
	bool m_EnableDraw = false;
	std::unique_ptr<CStaticMesh> m_Model;
	std::unique_ptr<CBox> m_BoxCollision;

public:
	CStageClearDoor(const Vector3& SetPos, const Vector3& SetRot);

	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	virtual void ShadowDraw() override;

	void EnableDraw(const bool Enable) { m_EnableDraw = Enable; }
};
