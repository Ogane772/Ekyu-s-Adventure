#pragma once
#include "../GameObject/SceneObject.h"
#include "../Calculation/Vector.h"
#include "../ImGui/Common/imgui.h"
#include <string>
#include <unordered_map>
#include <DirectXMath.h>
using namespace FrameWork;
using namespace DirectX;
enum class ESceneType;
enum class ETextureType;
enum class EEditorCreateObjectType;
enum class EStaticMeshType;
class CMouseCursor;
class CCamera;

class CImGuiEditor : public CSceneObject
{
private:
	//TextureTypeに紐づくImGui用テクスチャリスト
	std::unordered_map<ETextureType, ImTextureID> m_TextureList;
	int m_AutoSaveFrame = 0;
	static const int SAVE_FILE_NUM = 5;//各ステージのセーブデータの総数
	std::string m_SaveFileStrings[SAVE_FILE_NUM];//ImGuiが文字列はconst char* しか受け付けていないため文字列保存を行うようにstringの配列を用意する
	const char* m_SaveFiles[SAVE_FILE_NUM];
	int m_SelectSaveNumber = 0;//選択しているセーブデータの番号
	int m_SelectSceneNumber = 0;//選択しているシーンの番号
	std::string m_ObjectName;
	std::string m_SceneName;
	CMouseCursor* m_MouseCursor = nullptr;
	CCamera* m_Camera = nullptr;

	//テクスチャを登録
	void ImagePush(const ETextureType Type);
	//Commonオブジェクトの画像オブジェクト作成
	void EditCommonObjectImage(const EEditorCreateObjectType CreateType, const EStaticMeshType MeshType, const ETextureType TextureType, const char* Text, const Vector3& ModelScale);
	//Commonオブジェクト以外の画像オブジェクト作成及び配置個数の表示(配置個数がマイナスであれば表示しない)
	void EditObjectImage(const EEditorCreateObjectType CreateType, const ETextureType TextureType, const char* Text, const int Num = -1.0f);
	//シーン選択ウィンドウ表示
	void SceneChangeWindow();
	//選択しているオブジェクト情報表示ウィンドウ
	void SelectObjectWindow();
	//選択オブジェクトへのギズモ処理
	void EditTransform();
	//選択オブジェクトのコピーボタン表示と押下時の処理
	void EditCopy(const Vector3& Position, const Vector3& Rotation, const Vector3& Scale);
	//配置オブジェクト選択ウィンドウ
	void EditObjectWindow();
	//各オブジェクトに応じたウィンドウ情報
	void EditObjectTips(const Vector3& Position);
	void EditCameraSetting();
	void EditMaterial();
	void EditCullingSphere();
	void EditCollisionTag();
	void EditNormalPower();
	void EditBox();
	void EditCameraBox();
	void EditTutorialNumber();
	void EditEnemyMeshCollision();
	void EditEnemyFirstPosition(Vector3 pos);
	void EditEnemyTargetPoint();
	void EditEnemyRangeSphere();
	void EditEnemyRangeBox();
	void EditEnemyBodySphere();
	void EditRespawn();
	void EditItemDrop();
	void EditStarDoor();
	void EditAcornImage();
	void EditHeartImage();
	void EditStarImage();
	void EditStarDoorImage();
	void EditFistMapImage();
	void EditSecondMapImage();
	void EditSecondMapPlaneImage();
	void EditStageSelectImage();
	void EditPlaneImage();
	void EditNoMeshImage();
	void EditCollisionImage();
	void EditPlyerStartImage();
	void EditEnemy();
	void EditFirstBoss();

	//ドングリ配置タイプ設定。ここで設定後、ドングリを配置すると設定した内容で配置される
	void PutAcornWindow();
	//XMMATRIXをXMFLOAT4X4に変換
	XMFLOAT4X4 ConvertMatrixForXMFLOAT4X4(const XMMATRIX& Matrix);
	//ボックスのスライダーを編集
	void EditSliderBox();

public:
	virtual void Init() override;
	virtual void UnInit() override;
	virtual void Draw() override;
	void EditOff();
	
	void SetSaveFile(const ESceneType Type);
};