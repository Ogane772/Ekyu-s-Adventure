#pragma once
#include "../GameObject/GameObject.h"
#include "../Model/StaticMesh.h"

struct SphereData
{
	float Radius = 0.0f;
	Vector3 AddPosition;
};

class CSphere : public CGameObject
{
private:
	std::unique_ptr<CStaticMesh> m_Model;
	SphereData m_SphereData;
	void CollisionDraw();

public:
	CSphere(const Vector3& SetPos, const float Radius);
	CSphere() {};
	//モデルの頂点からスフィアを生成
	void AutoSphereCreate(const Vector3& SetPos, const Vector3& SetScale, const std::vector<std::vector<Vector3>>* Vertex);
	void Update(const Vector3 Position);
	virtual void Draw() override;
	void SingleDraw();
	void SetAddSpherePosition(const Vector3& AddPos);
	void SetRadius(const float Radius);
	virtual void ChangeSphereTexture(const ETextureType Type)override;
	float GetRadius() const { return m_SphereData.Radius; }
	Vector3 GetAddPosition() const { return m_SphereData.AddPosition; }
	//エディタでSphereの情報を操作するための取得関数
	SphereData& GetEditSphereData() { return m_SphereData; }
};