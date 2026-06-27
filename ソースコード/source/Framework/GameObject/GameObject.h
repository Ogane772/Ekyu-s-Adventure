#pragma once
#include "SceneObject.h"
#include "../Calculation/Vector.h"
#include <vector>
enum class ETextureType;
enum class ESoundIndex;
enum class EEffectType;
using namespace FrameWork;
class CSphere;
class CBox;
enum class EVibrationType;
struct ItemDropData;
struct MATERIAL;
struct ShaderNormalMapData;

class CGameObject : public CSceneObject
{
protected:
	int m_OriginalNumber = -1;//オブジェクト毎の固有ナンバー

	int m_Frame = 0;
	float m_FrameTime = 0.0f;
	
	XMMATRIX m_World;
	Vector3 m_Position;
	Vector3 m_DefaultPosition;
	Vector3 m_Rotation;
	Vector3 m_DefaultRotation;
	Vector3 m_Scale;
	
	//ベクトル
	Vector3 m_Front;//前ベクトル								  
	Vector3 m_Right;//右ベクトル
	Vector3 m_Up;//上ベクトル

	//カリング関連
	bool m_IsCulling = true;//カリング内に入ってたらtrue
	bool m_IsNoCullingCheck = false;//カリングしないならtrue
	bool m_IsOverrideCullingPosition = false;//カリング位置をm_Positionではなくm_OverrideCullingPositionを使用するか
	Vector3 m_OverrideCullingPosition;
	float m_CullingRadius = 0.0f;//カリング範囲

	//オブジェクトのUpdate、Draw前に呼び出される共通処理
	virtual void PreUpdate(float DeltaTime) override;
	virtual void PreDraw() override;
	virtual void PreShadowDraw() override;

	//trueで影を表示しない（エディタ用）
	bool m_IsEditorDisableShadow = false;
	//前、右、上フロントを作成
	void CreateVector();
	void TargetRotation(const float RotationSpeed, float DeltaTime, const Vector3 TargetRotation);
	//前、右、上フロントを更新（回転値を反映させる）
	void UpdateVector();
	void UpdateWorldMatrix();
	//オブジェクトリストにnullを代入
	void MapObjectNull(const int Number);

	void PlayVibration(const EVibrationType Type);
	UINT PlayEffect(const EEffectType Type, const Vector3& Position, const float Size);
	void StopEffect(const UINT EffectID);
	void UpdateEffectPosition(const UINT EffectID, const Vector3& Position);

public :
	virtual void SingleCollisonDraw() {};
	
	virtual XMMATRIX GetWorld() { return m_World; }
	
	virtual MATERIAL* GetMaterial() const { return nullptr; }

	//コリジョン判定開始ボックスを返す
	virtual CBox* GetCollisionBox() const { return nullptr; }

	UINT Play3DSoundIndex(const ESoundIndex SoundIndex, const Vector3& Position);
	void Update3DSoundPosition(const UINT Id, const Vector3& Position);

	virtual void SetScale(const Vector3& SetScale) { m_Scale = SetScale; }
	virtual void SetRotation(const Vector3& SetRot) { m_Rotation = SetRot; }
	void SetDefaultRotation(const Vector3& SetRot) { m_DefaultRotation = SetRot; }
	
	virtual void SetPosition(const Vector3& SetPos) { m_Position = SetPos; }

	Vector3 GetRotation() const { return m_Rotation; }
	virtual Vector3* GetRotationAddr() { return &m_Rotation; }
	Vector3 GetPosition() const { return m_Position; }
	Vector3* GetPositionAddr() { return &m_Position; }
	Vector3 GetScale() const { return m_Scale; }
	Vector3* GetScaleAddr() { return &m_Scale; }

	bool IsCulling() const { return m_IsCulling; }

	virtual CSphere* GetSphere() const { return nullptr; }
	virtual CSphere* GetRangeSphere() const { return nullptr; }
	virtual CBox* GetRangeBox() const { return nullptr; }
	
	virtual bool* IsDisableShadow() { return nullptr; }
	void SetEditorDisableShadow(const bool Disable) { m_IsEditorDisableShadow = Disable; }

	virtual bool* IsEnableLight() { return nullptr; }
	virtual void SetEnableLight(const bool Enable) {};

	virtual void ChangeTexture(const ETextureType Type, const int Index) {};
	virtual void ChangeSphereTexture(const ETextureType Type) {};

	void SetOriginalNumber(const int Number) { m_OriginalNumber = Number; }
	int GetOriginalNumber() const { return m_OriginalNumber; }
};