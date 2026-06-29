#pragma once
//各コリジョン判定を行うクラス
#include "../Calculation/Vector.h"
#include <vector>
using namespace FrameWork;

//当たり判定クラス
class CCapsule;
class CSphere;
class CBox;
class CCommonObject;
enum class ECollisionTagName;
enum class ECheckCollisionType;

struct HitResult
{
	Vector3* Position = nullptr; //ヒット判定に使用する座標(最終的に押し出し後の位置で上書きしたいためポインタで宣言)
	Vector3* HitNormal = nullptr; // 衝突面の法線ベクトル(値を格納したい変数を渡す)
	CCommonObject* HitObject = nullptr; // 衝突したオブジェクト

	HitResult() {};
	HitResult(Vector3* InPosition, Vector3* InHitNormal) : Position(InPosition), HitNormal(InHitNormal) {}
};

class CCollison
{
private:
	//////////////カプセル判定用////////////

	/// 点と線分の距離の平方を返す。
	/// 引数1 Point 点1の座標
	/// 引数2 LineStart 線分の端点1の座標
	/// 引数3 LineEnd 線分の端点2の座標
	static float DistancePointSegmentSq(const Vector3& Point, const Vector3& LineStart, const Vector3& LineEnd);

	/// 線分と線分の距離の平方を返す。
	/// 引数1 LineStartA 線分1の端点1の座標
	/// 引数2 LineEndA 線分1の端点2の座標
	/// 引数3 LineStartB 線分2の端点1の座標
	/// 引数4 LineEndB 線分2の端点2の座標
	static float DistanceSegmentSegmentSq(const Vector3& LineStartA, const Vector3& LineEndA, const Vector3& LineStartB, const Vector3& LineEndB);

	//カプセルの線とOBBの判定
	static bool CapsuleLineVsOBB(const CBox* Box, const CCapsule* Capsule);

	//////////////ボックス判定用////////////
	
	//ボックスと分離軸と物体間の距離を受け取り、影を算出し影がくっついていればtrueを返す
	static bool CompareLength(const CBox* BoxA, const CBox* BoxB, const Vector3& Separate, const Vector3& Distance);

	//点とOBBの最近接点を返す
	//引数1 点座標
	//戻り値 変換されたOBB中心座標
	static Vector3 ClosestPtPointOBB(const Vector3& PointPosition, const CBox* Box);

	//カプセルの円中心点とOBBを比較
	//引数3 カプセルの円中心点座標
	static bool CapsulePointVsOBB(const CCapsule* Capsule, const CBox* Box, const Vector3& PointPosition);
	
	static Vector3 Point_VS_Line(const Vector3& Point, const Vector3& LineStart, const Vector3& LineEnd);

	static bool CheckMeshCollision(const ECheckCollisionType Type, const std::vector<Vector3>& CollisionPositions, const float CollisionRadius, HitResult& InHitResult);

	//三角形の最近接点を返す
	/// 引数1 三角形と距離を測る点の座標
	/// 引数2 PointA 三角形の頂点1の座標
	/// 引数3 PointB 三角形の頂点2の座標
	/// 引数4 PointC 三角形の頂点3の座標
	static Vector3 ClosestPtPointTriangle(const Vector3& Position, const Vector3& PointA, const Vector3& PointB, const Vector3& PointC);

public:
	//メッシュとの当たり判定
	static bool Sphere_VS_Mesh(const ECheckCollisionType Type, const CSphere* Sphere, HitResult& InHitResult);
	static bool Capsule_VS_Mesh(const ECheckCollisionType Type, const CCapsule* Capsule, HitResult& InHitResult);
	//線とOBBの判定
	static bool OBBVsLine(CBox *Box, const Vector3& LineStart, const Vector3& LineEnd);
	//PointVsPoint
	/// 点と点の距離が一定以内であるかどうかを返す。
	/// 引数1 PointA 点1の座標
	/// 引数2 PointB 点2の座標
	/// 引数3 Distance 基準となる距離
	static bool CollisionCheck(const Vector3& PointA, const Vector3& PointB, float Distance);

	//CapsuleVsSphere
	/// 球と線分の距離が一定以内であるかどうかを返す。
	static bool CollisionCheck(const CCapsule* Capsule, const CSphere* Sphere);
	//LineVsSphere
	/// 線分と球の距離が一定以内であるかどうかを返す。
	static bool CollisionCheck(const Vector3& LineStart, const Vector3& LineEnd, const CSphere* Sphere);
	/// 線分とカプセルの距離が一定以内であるかどうかを返す。
	static bool CollisionCheck(const Vector3& LineStart, const Vector3& LineEnd, const CCapsule* Capsule);
	//CapsuleVsSphere
	/// 球と線分の距離が一定以内であるかどうかを返す。
	//ポジションを渡すとそのポジションに押し出し処理を行う
	static bool CollisionCheck(const CCapsule* Capsule, const CSphere* Sphere, Vector3* PositionA);

	//CapsuleVsPoint
	/// カプセル(線分）と点の距離が一定以内であるかどうかを返す。
	/// 引数2 Point 点座標
	static bool CollisionCheck(const CCapsule* Capsule, const Vector3& Point);

	//CapsuleVsOBB
	/// カプセルとボックスの距離が一定以内であるかどうかを返す。
	static bool CollisionCheck(const CCapsule* Capsule, const CBox* Box);

	//CapsuleVsCapsule
	/// カプセルとカプセルの距離が一定以内であるかどうかを返す。
	static bool CollisionCheck(const CCapsule* CapsuleA, const CCapsule* CapsuleB);

	//CapsuleVsCapsule
	/// カプセルとカプセルの距離が一定以内であるかどうかを返す
	//ポジションを渡すとそのポジションの親に押し出し処理を行う。
	static bool CollisionCheck(const CCapsule* CapsuleA, const CCapsule* CapsuleB, Vector3* PositionA);

	//SphereVsSphere
	/// 球と球の距離が一定以内であるかどうかを返す。
	static bool CollisionCheck(const CSphere* SphereA, const CSphere* SphereB);

	//SphereVsSphere
	/// 球と球の距離が一定以内であるかどうかを返す。
	//ポジションを渡すとそのポジションの親に押し出し処理を行う
	static bool CollisionCheck(const CSphere* SphereA, const CSphere* SphereB, Vector3* PositionA);

	//SphereVsPoint
	/// 球と点の距離が一定以内であるかどうかを返す。
	static bool CollisionCheck(const CSphere *Sphere, const Vector3& Point);

	//OBBVsOBB
	/// ボックスとボックスの距離が一定以内であるかどうかを返す。
	static bool CollisionCheck(const CBox *BoxA, const CBox *BoxB);

	//OBBVsSphere
	/// 球とボックスの距離が一定以内であるかどうかを返す。
	static bool CollisionCheck(const CBox* Box, const CSphere* Sphere);
};
