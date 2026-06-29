#pragma once
//ウィンドウのサイズやポーズで設定する値等の設定クラス
#include <d3d11.h>
#include <string>
#include <unordered_map>
#include "../Singleton/Singleton.h"
#include "../Calculation/Vector.h"
using namespace FrameWork;
enum class ECameraRotationSettingType;
enum class ECameraSpeedSettingType;
enum class ESceneType;

//画面サイズやポーズ、イベント等のシステムに関連するデータをまとめたクラス
class CSystemContext : public CSingleton<CSystemContext>
{
private:
	//シーンに紐づくロードするセーブデータ名のリスト
	std::unordered_map<ESceneType, std::string> m_SceneLoadSaveFileList;
	//シーンに紐づくシーン名のリスト
	std::unordered_map<ESceneType, std::string> m_SceneNameList;

	//画面サイズ関連
	const float m_DefaultScreenWidth = 1280.0f;//デフォルトのウィンドウの幅
	const float m_DefaultScreenHeight = 653.0f;//デフォルトのウィンドウの高さ
	float m_CurrentScreenWidth = m_DefaultScreenWidth;
	float m_CurrentScreenHeight = m_DefaultScreenHeight;
	HWND m_Window = HWND();//ウィンドウ情報
	RECT m_WindowRange = RECT();//ウィンドウの幅高さ情報

	//デバッグ用の描画切り替え
	bool m_IsDebugDisableAnimationModelRender = false;//true時アニメーションモデルの描画を切る
	bool m_IsDebugCollisionRender = false;//true時コリジョン等の当たり判定を表示する

	//ポーズ、エディットモード、イベント設定
	bool m_IsPause = false;
	bool m_IsEditMode = false;
	//イベント演出中true
	bool m_IsEvent = false;

	//ポーズ画面から設定できるカメラの回転/速度設定
	ECameraRotationSettingType m_CameraRotationUpAndDownType;
	ECameraRotationSettingType m_CameraRotationLeftAndRightType;
	ECameraSpeedSettingType m_CameraSpeedType;

	ECameraRotationSettingType ChangeCameraRotationSettingType(ECameraRotationSettingType Type);

public:
	CSystemContext();

	//シーンに紐づくセーブデータ名を取得
	std::string GetLoadSaveFile(const ESceneType Type);
	std::string GetSceneName(const ESceneType Type);

	//画面内に入っているか確認を行う。画面内：true 外：false 
	bool VFCulling(const Vector3& Position, const float Radius) const;

	//画面サイズの取得/変更
	void SetCurrentScreenWidth(const float Width) { m_CurrentScreenWidth = Width; }
	void SetCurrentScreenHeight(const float Height) { m_CurrentScreenHeight = Height; }
	float GetCurrentScreenWidth() const { return m_CurrentScreenWidth; }
	float GetCurrentScreenHeight() const { return m_CurrentScreenHeight; }
	float GetDefaultScreenWidth() const { return m_DefaultScreenWidth; }
	float GetDefaultScreenHeight() const { return m_DefaultScreenHeight; }
	float GetWidthDisplayScale() const { return m_CurrentScreenWidth / m_DefaultScreenWidth; };
	float GetHeightDisplayScale() const { return m_CurrentScreenHeight / m_DefaultScreenHeight; };
	void SetWindow(const HWND& Window) { m_Window = Window; }
	HWND GetWindow() const { return m_Window; }
	RECT GetWindowRange();

	//デバッグ用の描画の切り替え
	bool IsDebugDisableAnimationModelRender() const { return m_IsDebugDisableAnimationModelRender; }
	bool* IsDebugDisableAnimationModelRenderAddr() { return &m_IsDebugDisableAnimationModelRender; }
	void SetIsDebugCollisionRender(const bool Type) { m_IsDebugCollisionRender = Type; }
	bool IsDebugCollisionRender() const { return m_IsDebugCollisionRender; }
	bool* IsDebugCollisionRenderAddr() { return &m_IsDebugCollisionRender; }

	//ポーズ、イベント、エディットモードの切り替え
	void SetPause(const bool Type) { m_IsPause = Type; }
	bool IsPause() const { return m_IsPause; }
	void SetEvent(const bool Type) { m_IsEvent = Type; }
	bool IsEvent() const { return m_IsEvent; }
	void SetEditMode(const bool Type) { m_IsEditMode = Type; }
	bool IsEditMode() const { return m_IsEditMode; }
	bool* IsEditModeAddr() { return &m_IsEditMode; }

	//カメラの設定の取得/変更
	ECameraRotationSettingType GetCameraRotationUpAndDownType() const { return m_CameraRotationUpAndDownType; }
	ECameraRotationSettingType GetCameraRotationLeftAndRightType() const { return m_CameraRotationLeftAndRightType; }
	ECameraSpeedSettingType GetCameraSpeedType() const { return m_CameraSpeedType; }
	void ChangeCameraRotationUpAndDownType();
	void ChangeCameraRotationLeftAndRightType();
	void PrevCameraSpeedType();
	void NextCameraSpeedType();

	//引数で受け取ったモデルのファイルパスとテクスチャの絶対パスからテクスチャの相対パスに変換して返す
	void ConvertToTextureRelativePath(const std::string& FilePath, const std::string& AbsolutePath, std::string& OutRelativePath) const;
};