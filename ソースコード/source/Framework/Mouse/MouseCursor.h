#pragma once
//エディタを開いている際にオブジェクトの選択、配置等を行うクラス
#include "../GameObject/GameObject.h"
#include <memory>
class CCommonObject;
class CStaticMesh;
class CCamera;
class CLine;
class CBox;
enum class EStaticMeshType;
enum class EEditorCreateObjectType;
enum class EEditorSelectObjectType;
enum class ECollisionTagName;
enum class ECollisionTagCameraName;
enum class ECollisionTagSteps;

//マップに配置されているオブジェクト情報
struct MapObject 
{
	CGameObject* Object = nullptr;
	std::string Name;
	EStaticMeshType MeshType;
	EEditorSelectObjectType SelectObjectType;
	int Number = 0;
	int ObjectNumber = 0;//同オブジェクトがあった際、自身が何番目の同オブジェクトか格納
};

class CMouseCursor : public CGameObject
{
private:
	std::vector<MapObject> m_MapObjectList;//マップに配置しているオブジェクトのリスト
	int m_CreateNumber = 0;//生成オブジェクトの識別番号を格納
	int m_Number = 0;//生成オブジェクトへ割り振る番号

	//エフェクト生成中ならtrue
	bool m_EffectCreate = false;
	UINT m_EffectId = UINT_MAX;//ドラッグドロップ中のエフェクトIDを格納

	//ImGuiウィンドウの上にいたらtrue
	bool m_WantCaptureMouse = false;
	//通常時はマウスポインタ位置に球のモデルを表示。モデルのドラッグドロップ中はそのモデルを表示
	std::unique_ptr<CStaticMesh> m_StaticMesh;
	std::unique_ptr<CLine> m_Line;
	CCamera* m_Camera = nullptr;

	//選択したオブジェクトを保持するポインター
	CGameObject* m_SelectObject = nullptr;
	//生成するオブジェクトタイプを格納
	EEditorCreateObjectType m_CreateObjectType;
	//マップで選択しているオブジェクトタイプを格納する
	EEditorSelectObjectType m_SelectObjectType;
	//生成するオブジェクトのメッシュタイプ
	EStaticMeshType m_MeshType;

	//ドングリ配置用変数
	bool m_IsAcornLine = false;//線形配置ならtrue
	bool m_IsAcornCircle = false;;//円形配置ならtrue
	int m_AcornSetNumber = 10;//設置ドングリ数
	Vector3 m_EndAcornPosition = Vector3::Zero();//線形配置の時のドングリの終点位置（ローカルで指定)
	//円形配置の角度
	float m_CirclesAngle = 0.001f;
	//円形配置の円の半径
	float m_CirclesRadius = 8.0f;
	
	//選択しているオブジェクトのコリジョンのみ表示するか
	bool m_SingleCollisonDraw = false;

	//ドラッグドロップでオブジェクト配置
	void ObjectInstallation();
	//マップに配置されているオブジェクト取得
	void MapObjectCapture();
	//ドングリ生成
	void AcornCreate();
	//選択しているオブジェクト数CCommonObjectにキャストして返す
	CCommonObject* SelectCastCommonObject() const;

public:
	virtual void Init() override;
	//カメラからラインを飛ばしオブジェクトをチェックする
	void ObjectLineCheck(Vector3& Out, const Vector2& ScreenPoint, const Vector2& ScreenSize, const XMMATRIX& View, const XMMATRIX& Projection);
	// スクリーン座標をワールド座標に変換
	void CalcScreenToWorld(Vector3& Out, const Vector2& ScreenPoint, const float ProjectionZ, const Vector2& ScreenSize, const XMMATRIX& View, const XMMATRIX& Projection);
	virtual void Update(float DeltaTime) override;
	virtual void UnInit() override;
	virtual void Draw() override;

	//指定したオブジェクトを生成する
	void CreateObject(const EEditorCreateObjectType Type, const Vector3& Position);
	void SetMouseObject(CGameObject* Object, const EEditorSelectObjectType SelectType, const EStaticMeshType MeshType);
	//エディタでオブジェクトをドラッグドロップした時のオブジェクトを設定
	void StartDragDrop(const EEditorCreateObjectType Type);
	//選択している状態を解除する
	void ClearSelectObject();
	//エディタが閉じられたときに呼び出す
	void EditOff();
	//マップで選択しているオブジェクトを削除する
	void DeleteSelectObject();
	//オブジェクトリストにnullを代入
	void SetMapObjectNull(const int Number);
	virtual void SetScale(const Vector3& SetScale)override;
	//ドラッグドロップで配置するオブジェクトの大きさを変更
	void SetObjectScale(const Vector3 SetScale) { m_Scale = SetScale; };
	//m_SelectObjectがオブジェクトを持っていたらtrue
	bool IsSelectObject() const;
	//m_MapObjectListに要素を格納する
	void PushMapObject(CGameObject* Object, const std::string& ModelPath, const EStaticMeshType MeshType, const EEditorSelectObjectType SelectType);
	//CCommonObjectを生成する
	void CreateCommonObject(const EEditorCreateObjectType Type, const Vector3& Position, const Vector3& Rotation, const Vector3& Scale, const float Radius, const Vector3& SpherePosition, const ECollisionTagName PlayerCollision, const ECollisionTagName ObjectCollision, const ECollisionTagCameraName CameraCollision, const ECollisionTagSteps StepCollision, const bool IsLight, const MATERIAL* Material);
	//自動でそのモデルを囲むカリングスフィアを作成する
	void UpdateAutoCullingSphere();

	//各種ゲッター/セッター ポインタで返すものに関してはImGui側で変更が入る可能性がある
	virtual XMMATRIX GetWorld()override;
	virtual void SetRotation(const Vector3& SetRot)override;
	virtual void SetPosition(const Vector3& SetPos)override;
	virtual Vector3* GetRotationAddr()override;
	bool* IsAcornLine() { return &m_IsAcornLine; }
	bool* IsAcornCircle() { return &m_IsAcornCircle; }
	int* GetAcornSetNumber() { return &m_AcornSetNumber; }
	Vector3* GetEndAcornPosition() { return &m_EndAcornPosition; }
	float* GetCirclesAngle() { return &m_CirclesAngle; }
	float* GetCirclesRadius() { return &m_CirclesRadius; }
	EEditorCreateObjectType GetCreateObjectType() const { return m_CreateObjectType; }
	EEditorSelectObjectType GetSelectObjectType() const { return m_SelectObjectType; }
	void SetChoiceObjectNumber(const EEditorSelectObjectType Type);
	void SetPlayerCollisionTag(const ECollisionTagName CollisionTag);
	void SetObjectCollisionTag(const ECollisionTagName CollisionTag);
	void SetCameraCollisionTag(const ECollisionTagCameraName CollisionTag);
	void SetStepsCollisionTag(const ECollisionTagSteps CollisionTag);
	ECollisionTagName GetPlayerCollisionTag() const;
	ECollisionTagName GetObjectCollisionTag() const;
	ECollisionTagCameraName GetCameraCollisionTag() const;
	ECollisionTagSteps GetStepsCollisionTag() const;
	virtual MATERIAL* GetMaterial() const override;
	
	virtual bool* IsDisableShadow()override;
	bool IsLoadNormalMap() const;
	ShaderNormalMapData* GetNormalMapPower() const;
	EStaticMeshType GetMeshType() const { return m_MeshType; }
	void SetMeshType(const EStaticMeshType Type) { m_MeshType = Type; }
	virtual CBox* GetCollisionBox() const override;
	CSphere* GetCullingSphere() const;

	std::vector<Vector3>* GetTargetPoint();
	void AddTargetPosition(const Vector3& AddPos);
	void DeleteTargetPosition(const int index);
	void SetTargetPointImagePosition(const int index, const Vector3& SetPos);
	Vector3 GetFirstPosition();

	virtual CSphere* GetRangeSphere() const override;
	virtual CBox* GetRangeBox() const override;
	virtual CSphere* GetSphere() const override;

	bool* IsRespawn();
	int* GetRespawnTime();
	
	bool* IsNoCullingCheck();
	void SetNoCullingCheck(const bool Type);
	ItemDropData* GetItemDropData();
	virtual void ChangeTexture(const ETextureType Type, const int Index)override;
	int* GetOpenStar();
	bool* GetSingleCollisonDraw() { return &m_SingleCollisonDraw; }
	virtual void ChangeSphereTexture(const ETextureType Type)override;
	
	void MaterialAllSet(const MATERIAL& Material);//同じモデル番号のモデルに選択中のモデルのマテリアル情報セット
	const std::vector<MapObject>& GetMapObjectList() const { return m_MapObjectList; }
	int GetCreateNumber() const { return m_CreateNumber; }
	void SetCreateNumber(const int Number) { m_CreateNumber = Number; }
	void ResetMapObject();
	bool* IsEditorOnlyDraw();
	virtual bool* IsEnableLight()override;
	virtual void SetEnableLight(const bool Enable)override;
	bool* IsDisableSelfShadow() const;
	void SetDisableSelfShadow(const bool Disable);
	bool* IsEnableNormalMap() const;
	void SetEnableNormalMap(const bool Enable);
	void SetFirstPosition(const Vector3& SetPos);
	bool* IsBodySphereCollisionCheck();
	bool* IsDownLineCollisionCheck();
	bool* IsDisableDepth();
	float* GetHeightLockPosition();
	int* GetTutorialNumber() const;
	void SetAlpha(const float Alpha);

	//ドングリの円形配置
	/// 引数1 Index 現在の配置番号
	/// 引数2 CircleNum 配置する数
	/// 引数3 Angle 円自体の角度
	/// 引数4 Radius 円の半径の大きさ
	/// 引数5 Cx 円の中心座標をずらすX座標
	/// 引数6 Cz 円の中心座標をずらすZ座標
	//  戻り値 求められたXY座標
	Vector2 CirclesSet(const int Index, const int CircleNum, const float Angle, const float Radius, const float Cx, const float Cz);
	//ドングリの線形配置
	/// 引数1 Index 現在の配置番号
	/// 引数2 Num 配置する数
	/// 引数3 Start 始点位置
	/// 引数4 End 終点位置
	//  戻り値 求められたXYZ座標
	Vector3 LineSet(const int Index, const int Num, const Vector3& Start, const Vector3& End);
};
