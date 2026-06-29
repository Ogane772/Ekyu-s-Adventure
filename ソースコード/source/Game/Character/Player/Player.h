#pragma once
//プレイヤークラス
#include "../Character.h"
#include "../../../Framework/Line/Line.h"
#include <unordered_map>
#include <memory>

class CStaticMesh;
class CSkeletalMesh;
class CLine;
class CSphere;
class CCapsule;
class CPlayerStateMachine;
struct AnimationData;
enum class EPlayerAnimationNumber;

enum class ELandedState
{
	NONE,
	ON_LANDED,//着地時
	LAND,//着地中
	AIR,//空中,
};

enum class ENormalJumpState
{
	NO_JUMP,
	FIRST_JUMP,
	SECOND_JUMP,
	HIGH_JUMP
};

enum class EPlayerSpeedParameterType
{
	LAND_MOVE,
	LAND_TURN,
	CROUCHING_MOVE,
	CROUCHING_SLIDE,
	CROUCHING_JUMP,
	TAIL_ATTACK,
	AIR_MOVE,
	SPIN_JUMP,
	DIVE_JUMP,
	WALL_JUMP,
	DASH_JUMP,
	TURN_JUMP,
	WALL_HIT_MOVE,
};

class CPlayer : public CCharacter
{
private:
	//プレイヤーのステータス
	int m_MyAcornCount = 0;
	int m_MyStarCount = 0;
	std::unordered_map<EPlayerSpeedParameterType, SpeedParameter> m_SpeedParameterList;
	std::unique_ptr<CPlayerStateMachine> m_StateMachine;

	//坂にいる状態でジャンプを行った場合等の空中時に元の床の法線情報を使用する場合があるため保存
	Vector3 m_SaveDownNormal = Vector3::Zero();
	//床滑り重力
	Vector3 m_SlipGravity;
	//着地時の2段、3段ジャンプ受付時間をカウント
	float m_NextJumpTransitionTime = 0.0f;
	//ジャンプ状態を管理
	ENormalJumpState m_JumpState = ENormalJumpState::NO_JUMP;
	//スター取得時に表示するスターモデル
	std::unique_ptr<CStaticMesh> m_StarModel;

	//各種ライン
	std::unique_ptr <CLine> m_DownLine;
	std::unique_ptr<CLine> m_FrontLine;
	std::unique_ptr<CLine> m_CliffLine;
	std::unique_ptr<CLine> m_UpLine;

	//コリジョン
	std::unique_ptr<CCapsule> m_Capsule;//ボディコリジョン
	std::unique_ptr<CSphere> m_AttackSphere;//攻撃判定コリジョン

	//アニメーションリスト
	std::unordered_map<EPlayerAnimationNumber, AnimationData> m_AnimationDataList;
	//ラインヒット情報格納
	RayCastInfo m_DownCastInfo;
	RayCastInfo m_CliffCastInfo;

	//取得時のスターを表示している状態か
	bool m_EnableGetStarDraw = false;
	//ステージクリアになるスターを取得したか
	bool m_IsGetStageClearStar = false;
	//崖掴みに移動できないか
	bool m_DisableCliffState = false;
	//スピンジャンプを行った状態か(空中時は1回だけ行えるアクション)
	bool m_IsPlaySpinJumpAction = false;
	bool m_IsCapsuleHit = false;
	bool m_EnableClearButton = false;
	bool m_EnableCollision = true;
	ELandedState m_LandedState = ELandedState::LAND;

	//無敵時間時のカウント時間及び無敵時であることを表すフラグ
	float m_InvincibleCountTime = 0.0f;
	bool m_IsInvincible = false;
	//空中時にかかる力
	float m_JumpPower = 0.0f;
	//崖上がり時の移動先
	Vector3 m_CliffUpPosition = Vector3::Zero();
	Vector3 m_WallNormal = Vector3::Zero();
	
	void CheckBodyCollision();
	//移動中のSE再生
	void PlayMoveSE();
	//カメラ方向に向く
	void TurnTowardsCamera();

public:
	CPlayer();
	virtual void Init() override;
	virtual void UnInit() override;
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;	
	virtual void ShadowDraw() override;

	//アニメーション再生
	void PlayAnimationPlayer(const EPlayerAnimationNumber AnimationNumber, const bool IsBlend = true);
	bool IsAnimationEndPlayer(const EPlayerAnimationNumber AnimationNumber) const;
	void CheckLandMoveAnimation();
	
	//位置を初期位置に戻す
	void ResetDefaultPosition() { m_Position = m_DefaultPosition; }
	//移動を行う入力値がされたか
	bool IsInputMove() const;
	//カメラの向きを基準とした入力の値を取得
	Vector2 GetInputDirection() const;
	//入力による移動を行う
	void InputMove(const EPlayerSpeedParameterType Type, const float DeltaTime);
	//前方向のみに移動を行う
	void FrontMove(const EPlayerSpeedParameterType Type, const float DeltaTime, const bool IsReverseFront = false);
	//前方向のみを基準とした入力による移動を行う
	void FrontInputMove(const EPlayerSpeedParameterType Type, const float DeltaTime, const bool IsBreakMoveInput = false);

	//地形関連
	ELandedState CheckGround();//地面チェック
	void OnLanding();//着地時に呼び出す
	void UpdateLanding(float DeltaTime);//着地後に位置補正等を行う
	void CheckUpWallHit();
	bool IsUpWallHit() const;
	bool IsCliffHit();
	bool IsGround() const { return m_LandedState == ELandedState::LAND; }
	bool CheckWallSlip() const;
	void SetDisableCliffState(const bool Disable) { m_DisableCliffState = Disable; }
	//滑りベクトルを計算 引数1 入射ベクトル 引数2 対象法線 戻り値　滑りベクトル
	 Vector3 Slip(const Vector3& L, const Vector3& N);

	//落下/ジャンプ関連
	void CommonFallMove(float DeltaTime);//通常落下挙動
	void FallMove(float DeltaTime);
	void UpdateJumpFall(float DeltaTime);
	void ChangeFinalJumpLoop();
	bool IsJumpUp() const { return m_JumpPower > 0.0f; }
	bool IsFall() const { return m_JumpPower < 0.0f; }
	float GetJumpPower() const { return m_JumpPower; }
	void ResetJumpPower() { m_JumpPower = 0.0f; }
	void SetJumpState(const ENormalJumpState State) { m_JumpState = State; }
	void SetJumpAnimationAndJumpPower();
	bool IsSlopeSlip() const;
	bool CanSlopeSlideJump() const;
	void PlaySpinJumpAction();
	void ResetSpinJumpAction() { m_IsPlaySpinJumpAction = false; }

	//入力関連
	bool IsJumpTrigger() const;
	bool IsCliffUpTrigger() const;
	bool IsCliffDownTrigger() const;
	bool IsSpinJumpTrigger() const;
	bool IsDiveJumpTrigger() const;
	bool IsWallHit();
	bool IsHipDropTrigger() const;
	bool IsTailAttackTrigger() const;
	bool IsTurnInput();
	bool IsCrouchingPress() const;
	
	//各種ステートの開始/更新/終了
	void StartCliff();
	bool CliffUpMove(float CountTime);//崖上がりの移動を行う。終了時真を返す
	bool SpinJumpMove(const float CountTime, const float DeltaTime);//スピンジャンプの移動を行う。終了時真を返す
	void StartWallSlip();
	void WallSlipMove(const float DeltaTime);
	void StartWallJump();
	void StartWallHit();
	void StartHipDrop();
	void HipDropMove(const float DeltaTime);
	void StartDiveJump();
	void DiveJumpMove(const float DeltaTime);
	void StartTailAttack();
	void UpdateTailAttackCollision(const float CountTime);
	void StartTurnJump();
	void StartCrouchingJump();
	void StartDashJump();
	bool StartSlopeSlip();
	bool SlopeSlipMove(const bool IsCheckGroundSlipAnim, const float DeltaTime);
	void JumpSlipMove(const float DeltaTime);
	bool GroundSlipMove(const float DeltaTime);
	void StartCongratulation();
	bool EndCongratulation();
	void StartDeath();
	void EndDeath();
	void BackJumpMove(const EPlayerSpeedParameterType Type, const float DeltaTime);

	//外部からのステート切り替え
	void ChangeCongratulationState();
	void ChangeFallState();
	void ChangeNormalJumpState(const ENormalJumpState JumpState);

	//各種ステートチェック
	bool IsTurnJumpState() const;
	bool IsCrouchingJumpState() const;
	bool IsDamageState() const;
	bool IsCongratulationState() const;
	bool IsHipDropState() const;
	bool IsDiveJumpState() const;
	bool IsTailAttackState() const;
	bool IsDisableCliffState() const { return m_DisableCliffState; }
	bool IsEnemyKnockBackDamageState() const;
	bool IsEnemyFallDamageState() const;
	bool IsCliffState() const;
	bool IsNotInputState() const;//操作を行えないステート
	bool IsEnemyFallJumpState() const;//エネミーを踏んで倒した場合にジャンプを行うステート
	bool IsNormalJumpableState() const;//通常ジャンプ可能なステートか
	bool IsSpinJumpableState() const;//スピンジャンプ可能なステートか
	bool IsDiveJumpableState() const;//ダイブジャンプ可能なステートか
	bool IsHighJumpableState() const; // ハイジャンプ可能なステートか
	bool IsWallJumpableState() const; // 壁ジャンプ可能なステートか
	bool IsTurnJumpableState() const; // 振り向きジャンプ可能なステートか
	bool IsDashJumpableState() const; // ダッシュジャンプ可能なステートか
	bool IsTailAttackPossibleState() const; //尻尾アタック可能なステートか
	bool IsCrouchingPossibleState() const; //しゃがみ可能なステートか
	bool IsCrouchingJumpableState() const; //しゃがみジャンプ可能なステートか
	bool IsHipDropPossibleState() const;//ヒップドロップ可能なステートか

	//コリジョン関連
	void SetEnableCollision(const bool Enable) { m_EnableCollision = Enable; }
	void DisableAttackSphere();
	void UpdateFallAttackSphere();
	
	//音声関連
	void DestroyGroundSlipSE();
	void PlayDeathAnimeSE();
	void PlayGroundSlipSE();
	void DestroyWallSlipSE();
	void DestroyFinalJumpSE();
	void CheckMoveSE(float& CountSoundTime);//音声再生時、引数で受け取った値をリセットするため参照渡しにする

	//スター関連
	void DrawGetStar();
	void SetEnableGetStarDraw(const bool Enable) { m_EnableGetStarDraw = Enable; }
	bool IsEnableGetStar() const { return m_EnableGetStarDraw; }
	void StartStarDoorAnimation();
	bool IsGetStageClearStar() const { return m_IsGetStageClearStar; }
	bool IsSceneChangeGetStageClearStar() const;//シーン遷移を行うスター取得時にシーン遷移を開始する状態になったか返す
	void SetEnableGetStageClearStar() { m_IsGetStageClearStar = true; }

	//無敵関連
	void StartInvincible();
	void UpdateInvincibleCount(const float DeltaTime);
	bool IsBlinkInvincible() const;
	bool IsDamageOrDeath() const;
	
	//Yawの回転値を反転する
	void ReverseRotationYaw();
	
	//ダメージを受けた際に呼び出される
	virtual void Damage(const int Damage)override;

	//その他ゲッター/セッター
	int GetMaxHp() const { return m_HP.GetMaxHP(); }
	void AddHp(const int Add);
	void AddMyAcorn();
	void AddMyStar() { m_MyStarCount++; }
	void SetMyAcornCount(const int count) { m_MyAcornCount = count; }
	void SetMyStarCount(const int count) { m_MyStarCount = count; }

	CCapsule* GetCapsule() const;
	CSphere* GetAttackSphere() const;
	int GetMyAcornCount() { return m_MyAcornCount; }
	int GetMyStarCount() { return m_MyStarCount; }

	bool IsEnableClearButton() const { return m_EnableClearButton; }
	void SetEnableClearButton(const bool Enable) { m_EnableClearButton = Enable; };

	void SetDefaultPosition(const Vector3& SetPos);
	virtual void SetPosition(const Vector3& SetPos)override;

//デバッグ用処理
private:
	bool m_IsDebugNoDamage = false;//true時ダメージを受けない
	bool m_IsDebugNoItemHit = false;//true時ドングリとスターと当たり判定を行わない
	bool m_IsDebugNoStart = false;//true時プレイヤースタートから始まらない

public:
	bool* IsDebugNoDamage() { return &m_IsDebugNoDamage; }
	bool* IsDebugNoItemHit() { return &m_IsDebugNoItemHit; }
	bool* IsDebugNoStart() { return &m_IsDebugNoStart; }
};