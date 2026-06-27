#pragma once
#include "../Character.h"
#include "ItemDropTypes.h"
#include <vector>
#include "../../../Framework/Line/Line.h"

class CBillboard;
class CCapsule;

enum class EEnemyType
{
	SLIME,
	MONKEY,
	THORN_MAN,
	GORILLA,
	MAX
};

// 敵クラス
class CEnemy : public CCharacter
{
private:
	static UINT GenerateID();

protected:
	CPlayer* m_Player = nullptr;
	EEnemyType m_EnemyType;
	Vector3 m_FirstPosition;
	Vector3 m_FirstRotation;
	Vector3 m_FirstScale;
	//エネミーごとの固有番号
	int m_Id = 0;
	//現在移動している巡回位置番号
	int m_NowTargetPoint = 0;
	//巡回位置数
	int m_MaxTargetPoint = -1;

	//ターゲットポイント描画用ビルボード
	std::vector<std::unique_ptr<CBillboard>> m_TargetPointImage;
	std::vector<Vector3> m_TargetPoint;
	ItemDropData m_ItemDropData;
	//メッシュとの当たり判定フラグ
	bool m_IsBodySphereCollisionCheck = true;
	bool m_IsDownLineCollisionCheck = true;
	//1フレーム前の位置を格納
	Vector3 m_PrevPosition;

	//プレイヤーの方へ回転する
	void TowardThePlayer(const float RotationSpeed, float DeltaTime);

public:
	static CEnemy* Create(const EEnemyType Type, const Vector3& Position);
	CEnemy();
	virtual ~CEnemy();
	virtual void ShadowDraw()override;
	void TargetPointDraw();
	void AddTargetPosition(const Vector3& AddPos);
	void DeleteTargetPosition(const int index);
	void SetTargetPointImagePosition(const int index, const Vector3& SetPos);
	void UniInitTargetPointImage();
	void TailAttackDeath();
	void DiveAttackDeath();
	
	Vector3 GetFirstPosition() const { return m_FirstPosition; }

	virtual void PlayerHitCheck() {};

	virtual void SetBodySphere(const Vector3 local_pos, const float radius) {};
	virtual void SetRangeSphere(const Vector3 local_pos, const float radius) {};

	//巡回位置をローカルで指定
	//引数1 巡回位置の総数
	//引数2以降　巡回位置
	virtual void SetTargetPoint(int num, ...);
	virtual CCapsule* GetCapsule() const { return nullptr; }
	void CreateHitEffect(const Vector3 pos, const float scale);
	void CreateDeathEffect(const Vector3 pos, const float scale);

	UINT GetId() const { return m_Id; }
	int GetMaxTargetPoint() { return m_MaxTargetPoint; }

	bool* IsBodySphereCollisionCheck() { return& m_IsBodySphereCollisionCheck; }
	bool* IsDownLineCollisionCheck() { return&m_IsDownLineCollisionCheck; }
	void SetBodySphereCollisionCheck(const bool Enable) { m_IsBodySphereCollisionCheck = Enable; }
	void SetDownLineCollisionCheck(const bool Enable) { m_IsDownLineCollisionCheck = Enable; }
	
	Vector3 GetFirstRotation() { return m_FirstRotation; }
	Vector3 GetFirstScale() { return m_FirstScale; }
	
	std::vector<Vector3>* GetTargetPoint() { return &m_TargetPoint; }
	EEnemyType GetEnemyType()  { return m_EnemyType; }
	void SetFirstPosition(const Vector3& SetPos) {m_FirstPosition = SetPos; }
	virtual bool IsRespawnWait() const { return false; }

	//目標との距離を渡し、到着とみなす距離以内であれば真を返す
	bool IsTargetGoal(const Vector3& Direction) const;
	bool PlayerMove(SpeedParameter& SpeedParam, const float DeltaTime, const bool IsForcedBrake = false);
	bool UpdateMeshVsBodyCollision();
	bool UpdateGroundCollision(const float DeltaTime, RayCastInfo& OutRayCastInfo, const bool IsNoLineHitPositionReset = true);
	bool UpdateGroundCollision(const float DeltaTime);
};
