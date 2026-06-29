#pragma once
//コリジョンに使用するBoxクラス(OBB)
#include <memory>
#include "../GameObject/GameObject.h"
#include "../Model/StaticMesh.h"
#include "../Calculation/Matrix.h"

struct BoxData
{
	//max=ボックス右上 min=ボックス左下
	Vector3 Max;
	Vector3 Min;
	//XYZの各座標軸の傾きを表す方向ベクトル
	Vector3 AxisX;
	Vector3 AxisY;
	Vector3 AxisZ;
	//OBBの各座標軸に沿った長さの半分（中心点から面までの長さ）
	Vector3 Length;
	//ボックスの付与位置
	Vector3 AddPosition;
	//ボックスの回転値
	Matrix4 Rot;
};

class CBox : public CGameObject
{
private:
	std::unique_ptr<CStaticMesh> m_Model;
	BoxData m_BoxData;

	void CommonAutoBoxCreate(const bool IsAddBoxSize, const Vector3& Position, const Vector3& AddBoxSize, const Vector3& SetRot, const Vector3& SetScale, const std::vector<std::vector<Vector3>>* Vertex);
	void UpdateAxis();
	void CollisionDraw();

public:
	
	CBox(const Vector3& SetPos, const Vector3& Max, const Vector3& Min);
	CBox() {};

	//モデルの頂点からボックス生成
	void AutoBoxCreate(const Vector3& SetPos, const Vector3& SetRot, const Vector3& SetScale, const std::vector<std::vector<Vector3>>* Vertex);
	void AutoBoxCreate(const Vector3& AddBoxSize, const Vector3& SetPos, const Vector3& SetRot, const Vector3& SetScale, const std::vector<std::vector<Vector3>>* Vertex);
	void Update(const Vector3& Position, const Vector3& Rotation);
	virtual void Draw() override;
	void SingleDraw();
	void AddBoxPosition(const Vector3& addPos);
	void SetPosition(const Vector3& SetPos);
	//直接ポジションに足す
	void AddPosition(const Vector3 &AddPos);
	void SetBoxSize(const Vector3& Max, const Vector3& Min);
	Vector3 GetAxisX() const { return m_BoxData.AxisX; }
	Vector3 GetAxisY() const { return m_BoxData.AxisY; }
	Vector3 GetAxisZ() const { return m_BoxData.AxisZ; }
	Vector3 GetLength() const { return m_BoxData.Length; }
	Vector3 GetMax() const { return m_BoxData.Max; }
	Vector3 GetMin() const { return m_BoxData.Min; }
	Vector3 GetAddPosition() const { return m_BoxData.AddPosition; }
	//エディタでBoxの情報を操作するための取得関数
	BoxData& GetEditBoxData() { return m_BoxData; }
};