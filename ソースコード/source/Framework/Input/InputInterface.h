#pragma once
//キーボードとゲームパッドの入力を管理するクラス　ゲーム側からはこのクラスを経由して入力を取得する
#include <windows.h>
#include <map>
#include <queue>
#include "../Singleton/Singleton.h"
#include "../Calculation/Vector.h"
using namespace FrameWork;
class CInput;
class CGamePad;

enum class EVibrationType
{
	LOW,
	MIDDLE,
	HIGH,
	EXTRA_LARGE,
};

class CInputInterface : public CSingleton<CInputInterface>
{
private:
	struct VibrationParameter
	{
		float Time = 0.0f;//振動時間
		int Power = 0;//振動の強さ
		VibrationParameter() {};
		VibrationParameter(const float InTime, const int InPower) : Time(InTime), Power(InPower) {}
	};
	std::unique_ptr<CInput> m_Input;
	std::unique_ptr<CGamePad> m_GamePad;
	std::map<EVibrationType, VibrationParameter> m_VibrationParameters;

public:
	CInputInterface();
	~CInputInterface();

	void Update(float DeltaTime);

	//プレイヤーとカメラ関連のインプット情報
	Vector2 GetLeftStick() const;
	Vector2 GetRightStick() const;
	bool IsJumpTrigger() const;
	bool IsJumpPress() const;
	bool IsDiveJumpTrigger() const;
	bool IsDiveJumpPress() const;
	bool IsSpinJumpTrigger() const;
	bool IsCameraResetTrigger() const;
	bool IsInteractTrigger() const;
	bool IsSwimmingTrigger() const;
	bool IsSwimmingUpPress() const;
	bool IsCrouchingPress() const;
	bool IsHipDropTrigger() const;
	bool IsTailAttackTrigger() const;
	bool IsCliffUpTrigger() const;
	bool IsCliffDownTrigger() const;

	//UI関連のインプット情報
	bool IsDecisionTrigger() const;
	bool IsPauseTrigger() const;
	bool IsLeftSelectTrigger() const;
	bool IsRightSelectTrigger() const;
	bool IsUpSelectTrigger() const;
	bool IsDownSelectTrigger() const;

	//ImGui関連のインプット情報
	bool IsImGuiDrawTrigger() const;
	bool IsImGuiCaptureObjectTrigger() const;
	bool IsImGuiCaptureObjectDeleteTrigger() const;
	bool IsImGuiCaptureObjectRelease() const;
	bool IsImGuiSaveTrigger() const;

	//デバッグ操作用のインプット情報
	bool IsDebugCameraScreenClickTrigger() const;
	bool IsDebugCameraRotationPress() const;
	bool IsViewModelScreenClickTrigger() const;
	bool IsViewModelCameraRotationPress() const;
	float GetEditCameraUpDown() const;

	//カメラ振動
	void PlayVibration(const EVibrationType Type);
};
