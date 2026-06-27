#include "Collision.h"
#include "CollisionTag.h"
#include "Capsule.h"
#include "Sphere.h"
#include "Box.h"
#include "../GameObject/GameObject.h"
#include "../Object/CommonObject.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"

float CCollison::DistancePointSegmentSq(const Vector3& Point, const Vector3& LineStart, const Vector3& LineEnd)
{
	const Vector3 v1 = Point - LineStart;
	const Vector3 v2 = LineEnd - LineStart;
	const float v1v2 = Vector3::Dot(v1, v2);
	if (v1v2 <= 0.0f)
	{
		return v1.LengthSq();
	}
	const float v2v2 = v2.LengthSq();
	if (v2v2 <= v1v2)
	{
		const Vector3 v3 = Point - LineEnd;
		return v3.LengthSq();
	}
	return v1.LengthSq() - v1v2 * v1v2 / v2v2;
}

float CCollison::DistanceSegmentSegmentSq(const Vector3& LineStartA, const Vector3& LineEndA, const Vector3& LineStartB, const Vector3& LineEndB)
{
	// ねじれの位置の判定
	const Vector3 v1 = LineEndA - LineStartA;
	const Vector3 v2 = LineEndB - LineStartB;
	const Vector3 cross = Vector3::Cross(v1, v2);

	const float length = cross.LengthSq();
	if (length != 0.0f)
	{
		// 平行ではない
		const Vector3 v12 = LineStartB - LineStartA;
		const float nv12 = Vector3::Dot(cross, v12);
		const Vector3 vd = Vector3((nv12 / length)*cross.x, (nv12 / length)*cross.y, (nv12 / length)*cross.z);
		const Vector3 q1 = LineStartB - vd;
		const Vector3 q2 = LineEndB - vd;
		const Vector3 p1q1 = q1 - LineStartA;
		const Vector3 p1q2 = q2 - LineStartA;

		Vector3 r1, r2;
		r1 = Vector3::Cross(v1, p1q1);
		r2 = Vector3::Cross(v1, p1q2);

		if (Vector3::Dot(r1, r2) < 0.0f) 
		{
			const Vector3 v3 = q2 - q1;
			const Vector3 q1p1 = LineStartA - q1;
			const Vector3 q1p2 = LineEndA - q1;
			r1 = Vector3::Cross(v3, q1p1);
			r2 = Vector3::Cross(v3, q1p2);
			if (Vector3::Dot(r1, r2) < 0.0f)
			{
				// ねじれの位置
				return nv12 * nv12 / length;
			}
		}
	}
	// ねじれではない位置
	return min(
		min(DistancePointSegmentSq(LineStartA, LineStartB, LineEndB),
			DistancePointSegmentSq(LineEndA, LineStartB, LineEndB)),
		min(DistancePointSegmentSq(LineStartB, LineStartA, LineEndA),
			DistancePointSegmentSq(LineEndB, LineStartA, LineEndA))
	);
}

bool CCollison::CapsuleLineVsOBB(const CBox* Box, const CCapsule* Capsule)
{
	if (!Box || !Capsule)
	{
		return false;
	}
	const float EPSILON = 1.175494e-37f;

	const Vector3 downSpherePosition = Capsule->GetDownSpherePosition();

	Vector3 m = Capsule->GetUpSpherePosition() + downSpherePosition;
	m = m * 0.5f;
	
	const Vector3 boxAxisX = Box->GetAxisX();
	const Vector3 boxAxisY = Box->GetAxisY();
	const Vector3 boxAxisZ = Box->GetAxisZ();
	const Vector3 boxLength = Box->GetLength();

	Vector3 d = downSpherePosition - m;
	m = m - Box->GetPosition();
	m = Vector3(Vector3::Dot(boxAxisX, m),Vector3::Dot(boxAxisY, m), Vector3::Dot(boxAxisZ, m));
	d = Vector3(Vector3::Dot(boxAxisX, d),Vector3::Dot(boxAxisY, d), Vector3::Dot(boxAxisZ, d));
	float adx = fabsf(d.x);
	if (fabsf(m.x) > boxLength.x + adx)
	{
		return false;
	}
	float ady = fabsf(d.y);
	if (fabsf(m.y) > boxLength.y + ady)
	{
		return false;
	}
	float adz = fabsf(d.z);
	if (fabsf(m.z) > boxLength.z + adz)
	{
		return false;
	}
	adx += EPSILON;
	ady += EPSILON;
	adz += EPSILON;

	if (fabsf(m.y * d.z - m.z * d.y) > boxLength.y * adz + boxLength.z * ady)
	{
		return false;
	}
	if (fabsf(m.z * d.x - m.x * d.z) > boxLength.x * adz + boxLength.z * adx)
	{
		return false;
	}
	if (fabsf(m.x * d.y - m.y * d.x) > boxLength.x * ady + boxLength.y * adx)
	{
		return false;
	}

	return true;
}

bool CCollison::CompareLength(const CBox* BoxA, const CBox* BoxB, const Vector3& Separate, const Vector3& Distance)
{
	if (!BoxA || !BoxB)
	{
		return false;
	}
	const Vector3 boxA_AxisX = BoxA->GetAxisX();
	const Vector3 boxA_AxisY = BoxA->GetAxisY();
	const Vector3 boxA_AxisZ = BoxA->GetAxisZ();
	const Vector3 boxA_Length = BoxA->GetLength();
	const Vector3 boxB_AxisX = BoxB->GetAxisX();
	const Vector3 boxB_AxisY = BoxB->GetAxisY();
	const Vector3 boxB_AxisZ = BoxB->GetAxisZ();
	const Vector3 boxB_Length = BoxB->GetLength();

	//”分離軸上での”ボックスＡの中心からボックスＢの中心までの距離
	const float distance = fabsf(Vector3::Dot(Distance, Separate));
	//分離軸上でボックスAの中心から最も遠いボックスAの頂点までの距離
	float shadowA = 0;
	//分離軸上でボックスBの中心から最も遠いボックスBの頂点までの距離
	float shadowB = 0;
	//それぞれのボックスの”影”を算出
	shadowA = fabsf(Vector3::Dot(boxA_AxisX, Separate) * boxA_Length.x) +
		fabsf(Vector3::Dot(boxA_AxisY, Separate) * boxA_Length.y) +
		fabsf(Vector3::Dot(boxA_AxisZ, Separate) * boxA_Length.z);

	shadowB = fabsf(Vector3::Dot(boxB_AxisX, Separate) * boxB_Length.x) +
		fabsf(Vector3::Dot(boxB_AxisY, Separate) * boxB_Length.y) +
		fabsf(Vector3::Dot(boxB_AxisZ, Separate) * boxB_Length.z);

	if (distance > shadowA + shadowB)
	{
		return false;
	}
	return true;
}

Vector3 CCollison::ClosestPtPointOBB(const Vector3& PointPosition, const CBox* Box)
{
	if (!Box)
	{
		return Vector3();
	}
	const Vector3 boxAxisX = Box->GetAxisX();
	const Vector3 boxAxisY = Box->GetAxisY();
	const Vector3 boxAxisZ = Box->GetAxisZ();
	const Vector3 boxLength = Box->GetLength();

	Vector3 q = Box->GetPosition();
	Vector3 d = PointPosition - Box->GetPosition();
	float dist = Vector3::Dot(d, boxAxisX);

	if (dist > boxLength.x)
	{
		dist = boxLength.x;
	}
	if (dist < -boxLength.x)
	{
		dist = -boxLength.x;
	}
	Vector3 st = dist * boxAxisX;
	q = st + q;

	dist = Vector3::Dot(d, boxAxisY);

	if (dist > boxLength.y)
	{
		dist = boxLength.y;
	}
	if (dist < -boxLength.y)
	{
		dist = -boxLength.y;
	}
	st = dist * boxAxisY;
	q = st + q;

	dist = Vector3::Dot(d, boxAxisZ);

	if (dist > boxLength.z)
	{
		dist = boxLength.z;
	}
	if (dist < -boxLength.z)
	{
		dist = -boxLength.z;
	}
	st = dist * boxAxisZ;
	q = st + q;

	return q;
}

bool CCollison::CapsulePointVsOBB(const CCapsule* Capsule, const CBox* Box, const Vector3& PointPosition)
{
	if(!Capsule || !Box)
	{
		return false;
	}
	const Vector3 point = ClosestPtPointOBB(PointPosition, Box);
	const Vector3 v = point - PointPosition;
	const float dot = Vector3::Dot(v, v);
	const float radius = Capsule->GetRadius();
	return dot <= radius * radius;
}

Vector3 CCollison::Point_VS_Line(const Vector3& Point, const Vector3& LineStart, const Vector3& LineEnd)
{
	const Vector3 v1 = LineEnd - LineStart;
	const Vector3 v2 = Point - LineStart;
	const float length = v1.Length();
	const Vector3 norm = v1.Normalized();
	
	const float dot = Vector3::Dot(norm,v2);
	const float t = dot / length;	// 点から線分に垂直に下す際の割合

	Vector3 returnValue;

	if (t < 0.0f)
	{
		returnValue = v2;
	}
	else if (t > 1.0f)
	{
		returnValue = Point - LineEnd;
	}
	else
	{
		returnValue = Vector3(-((v1.x * t) + v2.x), -((v1.y * t) + v2.y), -((v1.z * t) + v2.z));
	}

	return returnValue;
}

Vector3 CCollison::ClosestPtPointTriangle(const Vector3& Position, const Vector3& PointA, const Vector3& PointB, const Vector3& PointC)
{
	const Vector3 ab = PointB - PointA;
	const Vector3 ac = PointC - PointA;
	const Vector3 ap = Position - PointA;
	const float d1 = Vector3::Dot(ab, ap);
	const float d2 = Vector3::Dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		return PointA;
	}

	const Vector3 bp = Position - PointB;
	const float d3 = Vector3::Dot(ab, bp);
	const float d4 = Vector3::Dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3)
	{
		return PointB;
	}

	const float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		return PointA + ab * v;
	}

	const Vector3 cp = Position - PointC;
	const float d5 = Vector3::Dot(ab, cp);
	const float d6 = Vector3::Dot(ac, cp);
	if (d6 >= 0.0f && d5 <= d6)
	{
		return PointC;
	}

	const float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		return PointA + ac * w;
	}

	const float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return PointB + (PointC - PointB) * w;
	}

	// 内部
	const float d00 = Vector3::Dot(ab, ab);
	const float d01 = Vector3::Dot(ab, ac);
	const float d11 = Vector3::Dot(ac, ac);
	const float d20 = Vector3::Dot(ap, ab);
	const float d21 = Vector3::Dot(ap, ac);
	const float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f)
	{
		return PointA;
	}
	const float v = (d11 * d20 - d01 * d21) / denom;
	const float w = (d00 * d21 - d01 * d20) / denom;
	return PointA + ab * v + ac * w;
}

bool CCollison::CheckMeshCollision(const ECheckCollisionType Type, const std::vector<Vector3>& CollisionPositions, const float CollisionRadius, HitResult& InHitResult)
{
	if (!InHitResult.Position || !InHitResult.HitNormal)
	{
		return false;
	}

	bool isHit = false;
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return false;
	}
	std::weak_ptr<CScene> scene = sceneManager->GetScene();
	if (scene.expired())
	{
		return false;
	}
	std::vector<CCommonObject*> models;
	scene.lock()->GetGameObjects<CCommonObject>(ESceneObjectType::MESH_OBJECT, models);

	// 重み付き合算用
	float totalWeight = 0.0f;
	Vector3 weightedPushSum = Vector3::Zero();
	Vector3 weightedNormalSum = Vector3::Zero();
	Vector3 normal = Vector3::Zero();

	for (CCommonObject* model : models)
	{
		if (Type == ECheckCollisionType::PLAYER)
		{
			if (!model->IsCulling() || model->GetPlayerCollisionTag() == ECollisionTagName::NO_LINE_HIT || model->GetPlayerCollisionTag() == ECollisionTagName::NO_COLLISION_HIT)
			{
				continue;
			}
		}
		else if (Type == ECheckCollisionType::OBJECT)
		{
			if (model->GetObjectCollisionTag() == ECollisionTagName::NO_COLLISION_HIT)
			{
				continue;
			}
			if (model->GetObjectCollisionTag() != ECollisionTagName::OBJECT_COLISION)
			{
				continue;
			}
		}

		const std::vector<CollisionMesh>* collision_mesh = model->GetNormalModelMeshCollision();
		const int size = (int)collision_mesh->size();

		// ワールド行列と逆転置（法線変換用）
		const XMMATRIX modelWorld = model->GetWorld();
		XMMATRIX normalMat = XMMatrixInverse(nullptr, modelWorld);
		normalMat = XMMatrixTranspose(normalMat);

		for (int i = 0; i < size; ++i)
		{
			const CollisionMesh& tri = (*collision_mesh)[i];

			// 三角形頂点をワールド空間へ変換（1回）
			Vector3 p0 = Vector3::TransformCoord(tri.Point[0], modelWorld);
			Vector3 p1 = Vector3::TransformCoord(tri.Point[1], modelWorld);
			Vector3 p2 = Vector3::TransformCoord(tri.Point[2], modelWorld);

			// 法線を逆転置でワールド変換
			Vector3 nW;
			nW.x = normalMat.r[0].m128_f32[0] * tri.Normal.x + normalMat.r[1].m128_f32[0] * tri.Normal.y + normalMat.r[2].m128_f32[0] * tri.Normal.z;
			nW.y = normalMat.r[0].m128_f32[1] * tri.Normal.x + normalMat.r[1].m128_f32[1] * tri.Normal.y + normalMat.r[2].m128_f32[1] * tri.Normal.z;
			nW.z = normalMat.r[0].m128_f32[2] * tri.Normal.x + normalMat.r[1].m128_f32[2] * tri.Normal.y + normalMat.r[2].m128_f32[2] * tri.Normal.z;
			nW.Normalize();

			for (const Vector3& collisionPosition : CollisionPositions)
			{
				Vector3 SC = collisionPosition;

				// 最近接点を求める
				const Vector3 closest = ClosestPtPointTriangle(SC, p0, p1, p2);
				const Vector3 diff = SC - closest;
				const float distSq = diff.LengthSq();
				const float radiusSq = CollisionRadius * CollisionRadius;

				// 当たってなければスキップ
				if (distSq >= radiusSq)
				{
					continue;
				}

				const float dist = sqrtf(distSq);
				const float penetration = CollisionRadius - dist; // 押し出し距離

				// 押し出し方向
				Vector3 pushDir;
				if (dist <= 1e-6f)
				{
					// 最近接点と中心がほぼ一致する場合は三角形法線にフォールバック
					// 三角形法線の向きを球心側へ向ける
					float sign = Vector3::Dot(SC - p0, nW);
					pushDir = (sign >= 0.0f) ? nW : (nW * -1.0f);
				}
				else
				{
					pushDir = diff * (1.0f / dist); // 正規化
				}

				// 押し出しベクトル（ワールド）
				const Vector3 pushVec = pushDir * penetration;

				// 重み付き合算
				weightedPushSum += pushVec * penetration;
				weightedNormalSum += pushDir * penetration;
				totalWeight += penetration;
				InHitResult.HitObject = model;
				isHit = true;
			}
		}
	}

	if (!isHit)
	{
		*InHitResult.HitNormal = Vector3(0.0f, 0.0f, 0.0f);
		return false;
	}

	// 重み付き平均を計算
	Vector3 averagePush = weightedPushSum;
	if (totalWeight > 0.0f)
	{
		averagePush = averagePush * (1.0f / totalWeight);
	}

	// 異常に大きな押し出しを防ぐための値
	const float maxPushLen = CollisionRadius * 1.0f;
	const float pushLen = averagePush.Length();
	if (pushLen > maxPushLen && pushLen > 0.0f)
	{
		averagePush = averagePush * (maxPushLen / pushLen);
	}

	// 最終適な位置
	*InHitResult.Position = *InHitResult.Position + averagePush;

	// 法線は重み付き合算を正規化して使用
	if (totalWeight > 0.0f)
	{
		Vector3 avgNormal = weightedNormalSum * (1.0f / totalWeight);
		avgNormal.Normalize();
		*InHitResult.HitNormal = avgNormal;
	}
	else
	{
		*InHitResult.HitNormal = Vector3(0.0f, 0.0f, 0.0f);
	}

	return true;
}

bool CCollison::Sphere_VS_Mesh(const ECheckCollisionType Type, const CSphere* Sphere, HitResult& InHitResult)
{
	if (!Sphere)
	{
		return false;
	}
	const std::vector<Vector3> collisionPositions = { Sphere->GetPosition() };
	return CheckMeshCollision(Type, collisionPositions, Sphere->GetRadius(), InHitResult);
}

bool CCollison::Capsule_VS_Mesh(const ECheckCollisionType Type, const CCapsule* Capsule, HitResult& InHitResult)
{
	if (!Capsule)
	{
		return false;
	}
	const std::vector<Vector3> collisionPositions = { Capsule->GetUpSpherePosition(), Capsule->GetDownSpherePosition() };
	return CheckMeshCollision(Type, collisionPositions, Capsule->GetRadius(), InHitResult);
}

bool CCollison::OBBVsLine(CBox* Box, const Vector3& LineStart, const Vector3& LineEnd)
{
	if (!Box)
	{
		return false;
	}
	const Vector3 boxAxisX = Box->GetAxisX();
	const Vector3 boxAxisY = Box->GetAxisY();
	const Vector3 boxAxisZ = Box->GetAxisZ();
	const Vector3 boxLength = Box->GetLength();

	const float EPSILON = 1.175494e-37f;

	Vector3 m = LineStart + LineEnd;
	m = m * 0.5f;

	Vector3 d = LineEnd - m;
	m = m - Box->GetPosition();
	m = Vector3(Vector3::Dot(boxAxisX, m), Vector3::Dot(boxAxisY, m), Vector3::Dot(boxAxisZ, m));
	d = Vector3(Vector3::Dot(boxAxisX, d), Vector3::Dot(boxAxisY, d), Vector3::Dot(boxAxisZ, d));

	float adx = fabsf(d.x);
	if (fabsf(m.x) > boxLength.x + adx)
	{
		return false;
	}
	float ady = fabsf(d.y);
	if (fabsf(m.y) > boxLength.y + ady)
	{
		return false;
	}
	float adz = fabsf(d.z);
	if (fabsf(m.z) > boxLength.z + adz)
	{
		return false;
	}

	adx += EPSILON;
	ady += EPSILON;
	adz += EPSILON;

	if (fabsf(m.y * d.z - m.z * d.y) > boxLength.y * adz + boxLength.z * ady)
	{
		return false;
	}
	if (fabsf(m.z * d.x - m.x * d.z) > boxLength.x * adz + boxLength.z * adx)
	{
		return false;
	}
	if (fabsf(m.x * d.y - m.y * d.x) > boxLength.x * ady + boxLength.y * adx)
	{
		return false;
	}

	return true;
}

bool CCollison::CollisionCheck(const Vector3& PointA, const Vector3& PointB, float Distance)
{
	const float dx = PointA.x - PointB.x;
	const float dy = PointA.y - PointB.y;
	const float dz = PointA.z - PointB.z;
	return dx * dx + dy * dy + dz * dz < Distance * Distance;
}

bool CCollison::CollisionCheck(const CCapsule* Capsule, const CSphere* Sphere)
{
	if (!Capsule || !Sphere)
	{
		return false;
	}
	const float cc = Sphere->GetRadius() + Capsule->GetRadius();
	return DistancePointSegmentSq(Sphere->GetPosition(), Capsule->GetUpSpherePosition(), Capsule->GetDownSpherePosition()) < cc * cc;
}

bool CCollison::CollisionCheck(const Vector3& LineStart, const Vector3& LineEnd, const CSphere* Sphere)
{
	if (!Sphere)
	{
		return false;
	}
	const float radius = Sphere->GetRadius();
	const float cc = radius + radius;
	return DistancePointSegmentSq(Sphere->GetPosition(), LineStart, LineEnd) < cc * cc;
}

bool CCollison::CollisionCheck(const Vector3& LineStart, const Vector3& LineEnd, const CCapsule* Capsule)
{
	if (!Capsule)
	{
		return false;
	}
	
	const float radius = Capsule->GetRadius();
	float cc = radius + radius;
	cc = cc * cc;
	bool isHit = DistancePointSegmentSq(Capsule->GetUpSpherePosition(), LineStart, LineEnd) < cc;
	if (isHit)
	{
		return true;
	}
	isHit = DistancePointSegmentSq(Capsule->GetDownSpherePosition(), LineStart, LineEnd) < cc;
	return isHit;
}

bool CCollison::CollisionCheck(const CCapsule* Capsule, const CSphere* Sphere, Vector3* PositionA)
{
	if (!Capsule || !Sphere)
	{
		return false;
	}
	const float cc = Sphere->GetRadius() + Capsule->GetRadius();
	const float length = DistancePointSegmentSq(Sphere->GetPosition(), Capsule->GetUpSpherePosition(), Capsule->GetDownSpherePosition());
	const bool isHit = length < cc * cc;
	// 当たってない場合リターン
	if (!isHit)
	{
		return isHit;
	}
	const Vector3 distance = Capsule->GetPosition() - Sphere->GetPosition();

	// 押し出し処理
	const float sqrtLength = sqrtf(length);
	const float pushLength = cc - sqrtLength;	// 押し出す距離
	const Vector3 pushVec = Vector3::Normalize(distance);	// 押し出す方向

	const Vector3 position = *PositionA;
	*PositionA = (position + (pushVec * pushLength));

	return isHit;
}

bool CCollison::CollisionCheck(const CCapsule* Capsule, const Vector3& Point)
{
	if (!Capsule)
	{
		return false;
	}
	const float radius = Capsule->GetRadius();
	const float cc = radius * radius;
	return DistancePointSegmentSq(Point, Capsule->GetUpSpherePosition(), Capsule->GetDownSpherePosition()) < cc;
}

bool CCollison::CollisionCheck(const CCapsule* CapsuleA, const CCapsule* CapsuleB)
{
	if (!CapsuleA || !CapsuleB)
	{
		return false;
	}
	const float length = DistanceSegmentSegmentSq(CapsuleA->GetUpSpherePosition(), CapsuleA->GetDownSpherePosition(), CapsuleB->GetUpSpherePosition(), CapsuleB->GetDownSpherePosition());
	const float cc = CapsuleA->GetRadius() + CapsuleB->GetRadius();
	return length < cc * cc;
}

bool CCollison::CollisionCheck(const CCapsule* CapsuleA, const CCapsule* CapsuleB, Vector3* PositionA)
{
	if (!CapsuleA || !CapsuleB)
	{
		return false;
	};
	const float length = DistanceSegmentSegmentSq(CapsuleA->GetUpSpherePosition(), CapsuleA->GetDownSpherePosition(), CapsuleB->GetUpSpherePosition(), CapsuleB->GetDownSpherePosition());
	const float cc = CapsuleA->GetRadius() + CapsuleB->GetRadius();
	const bool isHit = length < cc * cc;
	// 当たってない場合リターン
	if (!isHit)
	{
		return isHit;
	}
	const Vector3 distance = CapsuleA->GetPosition() - CapsuleB->GetPosition();
	// 押し出し処理
	const float sqrtLength = sqrtf(length);
	const float pushLength = cc - sqrtLength;	// 押し出す距離
	const Vector3 pushVec = Vector3::Normalize(distance);	// 押し出す方向

	const Vector3 position = *PositionA;
	*PositionA = (position + (pushVec * pushLength));

	return isHit;
}

bool CCollison::CollisionCheck(const CSphere* SphereA, const CSphere* SphereB)
{
	if (!SphereA || !SphereB)
	{
		return false;
	}
	
	const Vector3 posA = SphereA->GetPosition();
	const Vector3 posB = SphereB->GetPosition();
	const float length = (posA.x - posB.x) * (posA.x - posB.x) + (posA.y - posB.y) * (posA.y - posB.y) + (posA.z - posB.z) * (posA.z - posB.z);
	const float cc = SphereA->GetRadius() + SphereB->GetRadius();
	//2物体間の距離が、2物体の半径を足したものより小さいということは、
	//境界球同士が重なっている（衝突している）ということ
	if (length <= cc * cc)
	{
		return true;
	}
	return false;
}

bool CCollison::CollisionCheck(const CSphere* SphereA, const CSphere* SphereB, Vector3* PositionA)
{
	if (!SphereA || !SphereB)
	{
		return false;
	}
	const Vector3 distance = SphereA->GetPosition() - SphereB->GetPosition();
	const float lengthSq = distance.LengthSq();
	const float radiusLength = SphereA->GetRadius() + SphereB->GetRadius();

	const bool isHit = lengthSq <= radiusLength * radiusLength;
	// 当たってない場合リターン
	if (!isHit)
	{
		return isHit;
	}

	// 押し出し処理
	const float length = sqrtf(lengthSq);
	const float pushLength = radiusLength - length;	// 押し出す距離
	const Vector3 pushVec = Vector3::Normalize(distance);	// 押し出す方向

	const Vector3 position = *PositionA;
	*PositionA = (position + (pushVec * pushLength));

	return isHit;
}

bool CCollison::CollisionCheck(const CSphere* Sphere, const Vector3& Point)
{
	if (!Sphere)
	{
		return false;
	}
	const Vector3 posA = Sphere->GetPosition();
	const float length = (posA.x - Point.x) * (posA.x - Point.x) + (posA.y - Point.y) * (posA.y - Point.y) + (posA.z - Point.z) * (posA.z - Point.z);
	const float cc = Sphere->GetRadius();
	//2物体間の距離が、2物体の半径を足したものより小さいということは、
	//境界球同士が重なっている（衝突している）ということ
	if (length <= cc * cc)
	{
		return true;
	}
	return false;
}

bool CCollison::CollisionCheck(const CBox* BoxA, const CBox* BoxB)
{
	if (!BoxA || !BoxB)
	{
		return false;
	}
	const Vector3 boxA_Position = BoxA->GetPosition();
	const Vector3 boxA_AxisX = BoxA->GetAxisX();
	const Vector3 boxA_AxisY = BoxA->GetAxisY();
	const Vector3 boxA_AxisZ = BoxA->GetAxisZ();
	const Vector3 boxB_Position = BoxB->GetPosition();
	const Vector3 boxB_AxisX = BoxB->GetAxisX();
	const Vector3 boxB_AxisY = BoxB->GetAxisY();
	const Vector3 boxB_AxisZ = BoxB->GetAxisZ();

	//ボックスの距離ベクトル（中心と中心を結んだベクトル）
	const Vector3 distance = boxB_Position - boxA_Position;
	
	
	//ボックスAのローカル基底軸を基準にした、”影”の算出（3パターン）
	{
		//X軸を分離軸とした場合
		if (!CompareLength(BoxA, BoxB, boxA_AxisX, distance))
		{
			return false;
		}
		//Y軸を分離軸とした場合
		if (!CompareLength(BoxA, BoxB, boxA_AxisY, distance))
		{
			return false;
		}
		//Z軸を分離軸とした場合
		if (!CompareLength(BoxA, BoxB, boxA_AxisZ, distance))
		{
			return false;
		}
	}
	//ボックスBのローカル基底軸を基準にした、”影”の算出（3パターン）
	{
		//X軸を分離軸とした場合
		if (!CompareLength(BoxA, BoxB, boxB_AxisX, distance))
		{
			return false;
		}
		//Y軸を分離軸とした場合
		if (!CompareLength(BoxA, BoxB, boxB_AxisY, distance))
		{
			return false;
		}
		//Z軸を分離軸とした場合
		if (!CompareLength(BoxA, BoxB, boxB_AxisZ, distance))
		{
			return false;
		}
	}
	//お互いの基底軸同士の外積ベクトルを基準にした、”影”の算出（9パターン）
	//分離軸
	Vector3 separate;
	{
		//ボックスAのX軸
		{
			//と　ボックスBのX軸との外積ベクトルを分離軸とした場合
			separate = Vector3::Cross(boxA_AxisX, boxB_AxisX);
			if (!CompareLength(BoxA, BoxB, separate, distance))
			{
				return false;
			}
			//と　ボックスBのY軸との外積ベクトルを分離軸とした場合
			separate = Vector3::Cross(boxA_AxisX, boxB_AxisY);
			if (!CompareLength(BoxA, BoxB, separate, distance))
			{
				return false;
			}
			//と　ボックスBのZ軸との外積ベクトルを分離軸とした場合
			separate = Vector3::Cross(boxA_AxisX, boxB_AxisZ);
			if (!CompareLength(BoxA, BoxB, separate, distance))
			{
				return false;
			}
		}
		//ボックスAのY軸
		{
			//と　ボックスBのX軸との外積ベクトルを分離軸とした場合
			separate = Vector3::Cross(boxA_AxisY, boxB_AxisX);
			if (!CompareLength(BoxA, BoxB, separate, distance))
			{
				return false;
			}
			//と　ボックスBのY軸との外積ベクトルを分離軸とした場合
			separate = Vector3::Cross(boxA_AxisY, boxB_AxisY);
			if (!CompareLength(BoxA, BoxB, separate, distance))
			{
				return false;
			}
			//と　ボックスBのZ軸との外積ベクトルを分離軸とした場合
			separate = Vector3::Cross(boxA_AxisY, boxB_AxisZ);
			if (!CompareLength(BoxA, BoxB, separate, distance))
			{
				return false;
			}
		}
		//ボックスAのZ軸
		{
			//と　ボックスBのX軸との外積ベクトルを分離軸とした場合
			separate = Vector3::Cross(boxA_AxisZ, boxB_AxisX);
			if (!CompareLength(BoxA, BoxB, separate, distance))
			{
				return false;
			}
			//と　ボックスBのY軸との外積ベクトルを分離軸とした場合
			separate = Vector3::Cross(boxA_AxisZ, boxB_AxisY);
			if (!CompareLength(BoxA, BoxB, separate, distance))
			{
				return false;
			}
			//と　ボックスBのZ軸との外積ベクトルを分離軸とした場合
			separate = Vector3::Cross(boxA_AxisZ, boxB_AxisZ);
			if (!CompareLength(BoxA, BoxB, separate, distance))
			{
				return false;
			}
		}
	}
	return true;
}

bool CCollison::CollisionCheck(const CBox* Box, const CSphere* Sphere)
{
	if (!Box || !Sphere)
	{
		return false;
	}
	const Vector3 spherePos = Sphere->GetPosition();
	const Vector3 hitPoint = ClosestPtPointOBB(spherePos, Box);
	const Vector3 vector = hitPoint - spherePos;
	const float dot = Vector3::Dot(vector, vector);
	const float radius = Sphere->GetRadius();
	return dot <= radius * radius;
}

bool CCollison::CollisionCheck(const CCapsule* Capsule, const CBox* Box)
{
	if (!Capsule || !Box)
	{
		return false;
	}
	//まずスフィア1と2の当たり判定を調べる
	//その後線分で当たっているか調べる
	const Vector3 upSpherePosition = Capsule->GetUpSpherePosition();
	const Vector3 downSpherePosition = Capsule->GetDownSpherePosition();
	if (CapsulePointVsOBB(Capsule, Box, upSpherePosition))
	{
		return true;
	}
	if (CapsulePointVsOBB(Capsule, Box, downSpherePosition))
	{
		return true;
	}
	//スフィア1と2で当たらなかったらスフィア1と2の間の位置で調べる
	Vector3 addSpherePosition = downSpherePosition + upSpherePosition;
	addSpherePosition = addSpherePosition * 0.5f;
	if (CapsulePointVsOBB(Capsule, Box, addSpherePosition))
	{
		return true;
	}
	//最後にカプセルの線分とOBBの判定
	if (CapsuleLineVsOBB(Box, Capsule))
	{
		return true;
	}
	return false;
}
