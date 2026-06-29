#pragma once
//カメラクラス
#include "../GameObject/GameObject.h"
#include "../State/FunctionStateMachine.h"
#include "../Line/Line.h"
#include <memory>
#include <unordered_map>

enum class ECameraSpeedSettingType;
class CPlayer;

enum class ECameraState
{
	NORMAL,
	CAMERA_RESET,
	DEATH,
	STAGE_SELECT,
	EVENT,
};

enum class ECameraShakeType
{
	LOW,
	MIDDLE,
	HIGH,
};

class CCamera : public CGameObject
{
private:
	//カメラ振動構造体
	struct CameraShakeData
	{
		// カメラを揺らす時間(秒)
		float ShakeTime = 0.0f;
		// カメラの最大振動幅
		float ShakeAmplitude = 0.0f;
		// 何回振動を行う
		int ShakeCount = 0;

		CameraShakeData() {};
		CameraShakeData(const float InShakeTime, const float InShakeAmplitude, const int InShakeCount)
		{
			ShakeTime = InShakeTime;
			ShakeAmplitude = InShakeAmplitude;
			ShakeCount = InShakeCount;
		};
	};
	float m_CameraLength = 0.0f;//at(注視点)までの長さ 1以上

	//高さ固定用変数
	bool m_IsHeightLock = false;//高さを固定するか
	float m_HeightLockPosition;//固定する高さ

	//高さ補間用変数
	float m_LerpStartCameraHeight = 0.0f;//補間開始時の高さ
	float m_CountLerpTime = 0.0f;//高さ固定時に位置を補間で移動するため時間を計測する

	float m_CameraResetInterpValue = 0.0f;//カメラリセットの補間値 0.0-1.0が入る
	float m_Near = 1.0f;

	//エディットモード時の変数
	float m_EditCameraSpeed = 0.0f;
	float m_EditRightLeftRotSpeed = 0.0f;
	float m_EditUpDownRotSpeed = 0.0f;
	bool m_IsEditRightLeftRotChange = false;//エディタ中のカメラ操作の左右を反転するか
	bool m_IsEditUpDownRotChange = false;//エディタ中のカメラ操作の上下を反転するか
	POINT m_EditCameraCursorPos;//エディタモード中はカメラのドラッグ操作でカメラ回転を行えるようにする

	//カメラ演出中か
	bool m_IsCameraDirection = false;

	RECT m_Viewport;
	std::unique_ptr<CLine> m_FrontLine;
	XMFLOAT4X4 m_ViewMatrix;
	XMMATRIX	m_InvViewMatrix;
	XMMATRIX	m_ProjectionMatrix;
	Vector3 m_At;//カメラが見てる場所 注視点
	Vector3 m_CenterAt;//SetCameraCenterPosition()で設定される中視点
	CPlayer* m_TargetPlayer = nullptr;
	//ポーズ画面で設定するカメラ速度の種類に応じた速度リスト
	std::unordered_map<ECameraSpeedSettingType, float> m_CameraSpeedList;

	//一時的な値確保
	Vector3 m_SavePosition;
	Vector3 m_TemporaryFront;
	Vector3 m_DefaultFront;
	Vector3 m_TemporaryRotation;//カメラ補間のため、開始回転値を保持する

	//カメラ振動関連
	bool m_EnableCameraShake = false;
	// カメラ振動の経過時間(秒)
	float m_CountShakeTime = 0.0f;
	std::unordered_map<ECameraShakeType, CameraShakeData> m_ShakeDataList;
	CameraShakeData m_CurrentShakeData;

	CFunctionStateMachine<ECameraState> m_StateMachine;

	//カメラ位置リセット時の補間
	void UpdateCameraReset(float DeltaTime);
	
	//カメラの縦軸の回転値が制限値に到達しているか
	bool IsLimitCameraRotationY() const;

	//注視点、前ベクトル、カメラと注視点間の距離からカメラ位置を設定
	void SetupCameraPosition(const float CameraDistance);
	//カメラの位置をプレイヤーに更新
	void UpdateCameraPlayerPosition(float DeltaTime);

	//各種ステート関数
	void StartCameraReset();
	void UpdateGameCamera(float DeltaTime);
	void UpdateDebugCamera(float DeltaTime);
	void UpdateDeathCamera(float DeltaTime);
	void StartStageSelect();
	void UpdateEventCamera(float DeltaTime);

	//カメラ振動更新
	void UpdateCameraShake(float DeltaTime);

	float GetRotationSpeed();

public:
	virtual void Init() override;
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;

	//カメラをプレイヤー位置に戻す
	void ResetPlayerCamera();

	//カメラ演出時開始位置
	void StartCameraDirection(const Vector3 SetPos, const Vector3 SetFront, const float Length);
	//カメラ演出終了時にカメラをプレイヤー位置に戻す
	void EndCameraDirection();

	void SetTarget(CPlayer* Target) { m_TargetPlayer = Target; }

	void PlayCameraShake(const ECameraShakeType Type);

	float* GetCameraLength() { return &m_CameraLength; }

	void SetCameraLength(const float Length) { m_CameraLength = Length; }
	XMFLOAT4X4 GetViewMatrix() const { return m_ViewMatrix; }
	const XMMATRIX& GetInvViewMatrix() const{ return m_InvViewMatrix; }
	const XMMATRIX& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	
	void SetFront(const Vector3& Front) { m_Front = Front; };
	Vector3 GetFront() const { return m_Front; }
	void ResetCameraLength();

	float* GetEditCameraSpeed() { return &m_EditCameraSpeed; }
	float* GetEditRightLeftRotSpeed() { return &m_EditRightLeftRotSpeed; }
	float* GetEditUpDownRotSpeed() { return &m_EditUpDownRotSpeed; }
	bool* IsEditRightLeftRotChange() { return &m_IsEditRightLeftRotChange; }
	bool* IsEditUpDownRotChange() { return &m_IsEditUpDownRotChange; }
	void SetEditCameraSpeed(const float Speed) { m_EditCameraSpeed = Speed; }

	void SetHeightLock(const bool Type) { m_IsHeightLock = Type; m_CountLerpTime = 0.0f; m_LerpStartCameraHeight = m_At.y; }
	void SetHeightLockPosition(const float Position) { m_HeightLockPosition = Position; }

	void SetCameraPosition(const Vector3& AtPosition);
	void SetCameraCenterPosition(const Vector3& AtPosition);

	void ChangeState(const ECameraState State);
};
