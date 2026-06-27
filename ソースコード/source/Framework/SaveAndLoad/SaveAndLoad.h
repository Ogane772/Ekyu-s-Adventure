#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "../Singleton/Singleton.h"
#include "../../Game/Character/Enemy/ItemDropTypes.h"
#include "../Renderer/ShaderStruct.h"
#include "../Calculation/Vector.h"
using namespace FrameWork;
enum class EEditorSelectObjectType;
enum class ECollisionTagName;
enum class ECollisionTagCameraName;
enum class ECollisionTagSteps;
enum class ESceneType;
enum class EStaticMeshType;
class CBox;
class CSphere;
class CEnemy;

class CSaveAndLoad : public CSingleton<CSaveAndLoad>
{
private:
	//各オブジェクトのセーブデータ
	struct SaveData 
	{
		EEditorSelectObjectType ObjectType;//生成するオブジェクトのタイプ
		Vector3 Position;
		Vector3 Scale;
		Vector3 Rotation;
		MATERIAL Material;//モデルのマテリアル情報

		//カリングを行わないか
		int NoCullingCheck = 0;

		//エディタを開いているときのみ表示するか
		int EditorOnlyDraw = 0;

		//ライトを適用するか
		int EnableLight = 0;

		//カリングやプレイヤー探知用のスフィアの半径、位置
		float SphereRadius = 0.0f;
		Vector3 SpherePosition;
		
		//ボックスコリジョンの大きさ、位置
		Vector3 BoxMax;
		Vector3 BoxMin;
		Vector3 BoxPosition;

		//CommonObject用のデータ
		EStaticMeshType StaticMeshType;//CommonObjectにおいて生成するモデル
		//オブジェクトのコリジョン
		ECollisionTagName PlayerCollisionTag;
		ECollisionTagName ObjectCollisionTag;
		ECollisionTagCameraName CameraCollisionTag;
		ECollisionTagSteps StepsCollisionTag;
		int DisableShadow = 0;//影を使用しないか
		int EnableNormalMap = 0;//ノーマルマップを使用するか
		float NormalPower = 0.0f;//ノーマルマップの強さ
		float NormalSpecular = 0.0f;//ノーマルマップのスペキュラ
		int DisableDepth = 0;//深度を無効にするか
		int DisableSelfShadow = 0;//自身に影を当てないようにするか

		//Enemy用のデータ
		float BodySphereRadius = 0.0f;//ボディコリジョンに使用するスフィアの大きさ
		Vector3 BodySpherePosition;//ボディコリジョンに使用するスフィアの位置
		int TargetPointMax = 0;//移動経路に使用するターゲットポイントの総数
		std::vector<Vector3> TargetPoint;//移動経路に使用するターゲットポイントの位置リスト
		int IsRespawn = 0;//リスポーンするか
		int RespawnTime = 0;//リスポーン時間
		int IsBodyCollisionCheck = 0;//ボディコリジョンを使用しメッシュと当たり判定を取るか
		int IsDownLineCollisionCheck = 0;//ラインを使用し、地面と当たり判定を取るか
		int AttackFrame = 0;//攻撃を開始する時間
		ItemDropData ItemDrop;//アイテムドロップ情報
		
		//ステージクリアに必要なスターの数
		int OpenStar = 0;
			
		//コリジョンに触れたときにカメラを設定した高さに固定する
		float HeightLockPosition = 0.0f;
		
		//コリジョンに触れたときに番号に紐づくチュートリアルを表示する
		int TutorialNumber = 0;
	};
	//プレイヤー用のセーブデータ
	struct PlayerSaveData
	{
		int StageStarCount = 0;//そのステージで取得したスター数
		int StageAcornCount = 0;//そのステージで取得したドングリ数

		PlayerSaveData() {};
		PlayerSaveData(const int Star, const int Acorn)
		{
			StageStarCount = Star;
			StageAcornCount = Acorn;
		}
	};
	//一時的な個数保持
	//シーンを跨いで保持する変数
	int m_TemporaryStarCount;
	int m_TemporaryAcornCount;
	int m_TemporaryHP;

	//メインゲーム→ボスと続き、シーンが変わってしまうためメインゲーム部分の開始シーンを格納(ドングリとスターをシーンに紐づきセーブするため)
	ESceneType m_StartMainGameScene;
	SaveData m_SaveData;

	std::unordered_map<ESceneType, PlayerSaveData> m_PlayerSaveData;
	void CommonLoad(const std::string& FileName);
	void ObjectLoad(std::string& LineData);
	void SatBoxData(CBox* Box);
	void SatSphereData(CSphere* Sphere);
	void SatBodySphereData(CSphere* Sphere);
	void SetTargetPositionData(std::string& LineData, CEnemy* Enemy);

public:
	void Save(const std::string& FileName);
	void PlayerSave();
	void PlayerLoad();
	//ロードするもの全てを一回リリースしてから配置
	void Load(const std::string& FileName);
	//今のマップに追加配置
	void AddLoad(const std::string& FileName);
	void SetStartMainGameScene(const ESceneType Type) { m_StartMainGameScene = Type; }
	void SetTemporaryItemCount(const int Star, const int Acorn)
	{
		m_TemporaryStarCount = Star;
		m_TemporaryAcornCount = Acorn;
	}

	void SetMyStarAndAcorn(const ESceneType Type, const int Star, const int Acorn);
	int GetTemporaryStarCount() const { return m_TemporaryStarCount; }
	int GetTemporaryAcornCount() const { return m_TemporaryAcornCount; }
	int GetStarCount(const ESceneType Type);
	int GetAcornCount(const ESceneType Type);
	ESceneType GetStartMainGameScene() const { return m_StartMainGameScene; }
};
