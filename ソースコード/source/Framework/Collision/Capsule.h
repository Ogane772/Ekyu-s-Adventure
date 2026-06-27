#pragma once
#include "../GameObject/GameObject.h"
#include <d3d11.h>
struct VERTEX_3D;
class CStaticMesh;
class CTextureLoader;

class CCapsule : public CGameObject
{
private:
	struct CapsuleData
	{
		Vector3 UpSpherePosition;//カプセルの上部分の半球の中心座標
		Vector3 DownSpherePosition;//カプセルの下部分の半球の中心座標
		Vector3 CapsuleVector;//分割方向を決定するためのベクトル
		float Radius = 0.0f;//半径
	};

	CapsuleData m_CapsuleData;
	Vector3 DefaultUpSpherePosition;//初期値のカプセルの上部分の半球の中心座標
	Vector3 DefaultDownSpherePosition;//初期値のカプセルの下部分の半球の中心座標

	//カプセルの描画に必要なデータ
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;
	UINT m_IndexNum = 0;
	int m_VertexNum = 0;
	std::weak_ptr<ID3D11ShaderResourceView> m_Texture;
	std::unique_ptr<CStaticMesh> m_Model;

	/// カプセル上の点をセットする
	/// @param Position 基準座標
	/// @param Vector 基準からの目標点に向かうベクトル
	void SetVertex(VERTEX_3D* Vertex, const Vector3& Position, const Vector3& Vector);
	void CollisionDraw();

public:
	
	/// 任意の大きさのカプセル型のメッシュを作成。
	/// @param UpSpherePosition         カプセルの上部分の半球の中心座標
	/// @param DownSpherePosition         カプセルの下部分の半球の中心座標
	/// @param Vector          分割方向を決定するためのベクトル(p1-p2と平行でないベクトルを指定)
	/// @param Radius     半径
	/// @param Slices     軸を中心とした分割数
	/// @param Stacks_1_2 半球部分の分割数
	///
	CCapsule(const Vector3& UpPosition, const Vector3& DownPosition, const Vector3& Vector, const float Radius, const UINT Slices, const UINT Stacks_1_2);
	~CCapsule();

	void Update(const Vector3 Position);
	virtual void Draw() override;
	void SingleDraw();
	void SetCapsulePosition(const Vector3& UpSpherePosition, const Vector3& DownSpherePosition);
	void SetCapsuleSize(const Vector3& UpSpherePosition, const Vector3& DownSpherePosition, const  float Radius);
	float GetRadius() const { return m_CapsuleData.Radius; }
	Vector3 GetUpSpherePosition() const { return m_CapsuleData.UpSpherePosition; }
	Vector3 GetDownSpherePosition() const { return m_CapsuleData.DownSpherePosition; }
};
