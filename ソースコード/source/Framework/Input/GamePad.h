#pragma once
//ゲームパッドクラス　XInputを使用
#include <XInput.h>
#include "../Calculation/Vector.h"
using namespace FrameWork;

class CGamePad
{
private:
	//振動中ならtrue
	bool m_IsVibration = false;
	//振動する時間を格納
	float m_VibrationTime = 0.0f;
	float m_VibrationCountTime = 0.0f;
	//振動の強さを格納
	int m_VibPower;
	XINPUT_STATE m_InputState{};
	XINPUT_STATE m_PrevInputState = m_InputState;

	void UpdateVibration(float DeltaTime);
	//スティックの値を-1.0f～1.0fの値に変換を行う
	Vector2 ClampStick(const SHORT InputVectorX, const SHORT InputVectorY) const;
	float NormalizeAxis(const SHORT InputVector) const;

public:

	void Update(float DeltaTime);

	bool IsButtonTrigger(const int Key) const;
	bool IsButtonPress(const int Key) const;
	bool LTButtonTrigger() const;
	bool LTButtonPress() const;
	bool RTButtonTrigger() const;
	bool RTButtonPress() const;

	//ゲームパッドプレス処理
	//左スティック上傾きトリガー処理
	bool LStickYTriggerCheckUp() const;
	//左スティック下傾きトリガー処理
	bool LStickYTriggerCheckDown() const;
	//左スティック左傾きトリガー処理
	bool LStickXTriggerCheckLeft() const;
	//左スティック右傾きトリガー処理
	bool LStickXTriggerCheckRight() const;

	//コントローラーを振動させる　引数1 振動時間　引数2 振動の強さ
	void PlayVibration(const float Time ,const int Power);
	Vector2 GetLeftStick();
	Vector2 GetRightStick();
};