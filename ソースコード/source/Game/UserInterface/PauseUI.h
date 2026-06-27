#pragma once
#include "../../Framework/GameObject/SceneObject.h"
#include "../../Framework/State/FunctionStateMachine.h"

class C2DPolygon;

class CPauseUI : public CSceneObject
{
private:
	enum class EOpenState
	{
		GAME_BACK,
		OPTION,
		TITLE_BACK,
	};
	enum class EOptionState
	{
		CAMERA_LEFT_AND_RIGHT,
		CAMERA_UP_AND_DOWN,
		CAMERA_SPEED,
		BACK,
	};
	enum class ECheckTitleState
	{
		RETURN_TITLE,
		BACK,
	};
	enum class EPauseState
	{
		NONE,
		OPEN,
		OPTION,
		CHECK_TITLE
	};
	int m_SelectNumber = 0;
	CFunctionStateMachine<EPauseState> m_StateMachine;
	std::unique_ptr<C2DPolygon> m_BG;
	std::unique_ptr<C2DPolygon> m_PauseText;
	std::unique_ptr<C2DPolygon> m_GameBackText;
	std::unique_ptr<C2DPolygon> m_OptionText;
	std::unique_ptr<C2DPolygon> m_OptionRedText;
	std::unique_ptr<C2DPolygon> m_TitleBackText;
	std::unique_ptr<C2DPolygon> m_TitleBackRedText;
	std::unique_ptr<C2DPolygon> m_CameraUpAndDown;
	std::unique_ptr<C2DPolygon> m_CameraLeftAndRight;
	std::unique_ptr<C2DPolygon> m_CameraSpeed;
	std::unique_ptr<C2DPolygon> m_CameraReverse;
	std::unique_ptr<C2DPolygon> m_CameraHigh;
	std::unique_ptr<C2DPolygon> m_CameraBack;
	std::unique_ptr<C2DPolygon> m_CameraSlow;
	std::unique_ptr<C2DPolygon> m_CameraNormal;
	std::unique_ptr<C2DPolygon> m_CameraSpeedNormal;
	std::unique_ptr<C2DPolygon> m_CameraArrow;
	std::unique_ptr<C2DPolygon> m_Yes;
	std::unique_ptr<C2DPolygon> m_No;

	void StartOpen();
	void UpdateOpen(float DeltaTime);
	void DrawOpen();
	void StartOption();
	void UpdateOption(float DeltaTime);
	void DrawOption();
	void StartCheckTitle();
	void UpdateCheckTitle(float DeltaTime);
	void DrawCheckTitle();

	//オプション画面で左か右が押されたときに項目を切り替える処理
	void InputLeftOrRightOption(const bool IsRightInput);

public:
	
	virtual void Init() override;
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
};
