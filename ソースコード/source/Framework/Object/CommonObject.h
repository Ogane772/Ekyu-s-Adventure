#pragma once
//壁等の固有の挙動が無いスタティックメッシュを描画する汎用クラス
#include "../GameObject/GameObject.h"
#include "../Collision/Sphere.h"
#include <vector>
#include <memory>

class CStaticMesh;
struct CollisionMesh;
enum class EStaticMeshType;
enum class ECollisionTagName;
enum class ECollisionTagCameraName;
enum class ECollisionTagSteps;

class CCommonObject : public CGameObject
{
private:
	//trueでエディタ時のみ描画
	bool m_IsEditorOnlyDraw = false;
	std::unique_ptr<CSphere> m_CullingSphere;//カリング用のスフィアコリジョン
	std::unique_ptr<CStaticMesh> m_Model;//描画モデル
	XMMATRIX m_MeshWorld;//モデルのワールドマトリクス
	bool m_IsDisableShadow = false;//真のとき影を表示しない
	bool m_IsDisableDepth = false;//真の時深度を考慮しない
	//自身が何かしらのクラスに保持されていればtrue(エディタで選択されたか判定を取られないようにするため)
	bool m_ObjectReference = false;

	//プレイヤーが参照するコリジョンタグ
	ECollisionTagName m_PlayerCollisionTag;
	//エネミーやオブジェクトが参照するコリジョンタグ
	ECollisionTagName m_ObjectCollisionTag;
	//カメラのコリジョンタグ
	ECollisionTagCameraName m_CameraCollisionTag;
	//足音のコリジョンタグ
	ECollisionTagSteps m_StepsCollisionTag;

public:
	
	void Init(const EStaticMeshType Type, const Vector3& SetPos, const Vector3& SetScale, const Vector3& SetRot);
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	virtual void SingleCollisonDraw() override;
	virtual void ShadowDraw() override;

	//カリング用のスフィア生成・更新・設定・取得
	void CreateCullingSphere(const float Radius, const Vector3& SetPos);
	void CreateAutoCullingSphere();//モデルの頂点に合わせたカリングスフィアを自動で生成する
	void UpdateAutoCullingSphere();
	void SetCullingRadius(const float Radius);
	CSphere* GetCullingSphere() const { return m_CullingSphere.get(); }

	//影の無効設定
	virtual bool* IsDisableShadow() override { return &m_IsDisableShadow; }
	void SetDisableShadow(const bool Disable) { m_IsDisableShadow = Disable; }
	//モデルへの影のみ無効設定
	bool* IsDisableSelfShadow() const;
	void SetDisableSelfShadow(const bool Disable);

	//位置・回転設定
	virtual void SetPosition(const Vector3& SetPos) override;
	virtual void SetRotation(const Vector3& SetRot) override;

	//モデルがノーマルマップを使用しているか
	bool IsLoadNormalMap() const;
	//ノーマルマップの強さを取得・設定
	ShaderNormalMapData* GetNormalMapPower() const;
	void SetNormalMapPower(ShaderNormalMapData& NormalMapData);
	//モデルがノーマルマップを使用しているか取得・設定
	bool* IsEnableNormalMap() const;
	void SetEnableNormalMap(const bool Enable);

	//自身が何かしらのクラスに保持されているか
	void SetObjectReference() { m_ObjectReference = true; }
	bool GetObjectReference() { return m_ObjectReference; }

	//モデルの種類を取得
	EStaticMeshType GetMeshType() const;
	//モデルのメッシュコリジョンを取得
	std::vector<CollisionMesh>* GetNormalModelMeshCollision();
	//モデルのマテリアル情報を取得・設定
	virtual MATERIAL* GetMaterial() const override;
	void SetMaterial(const MATERIAL& Material);

	//モデルのワールドマトリクスを取得
	virtual XMMATRIX GetWorld() override { return m_MeshWorld; }

	//カリングを行うか取得・設定
	bool* IsNoCullingCheck() { return &m_IsNoCullingCheck; }
	void SetNoCullingCheck(const bool Type);

	//エディタ中の時のみモデルを表示するか取得・設定
	bool* IsEditorOnlyDraw() { return &m_IsEditorOnlyDraw; }
	void SetEditorOnlyDraw(const bool Enable) { m_IsEditorOnlyDraw = Enable; }
	
	//指定したテクスチャを設定する
	virtual void ChangeTexture(const ETextureType Type, const int Index)override;
	//カリングスフィアのテクスチャを設定する
	virtual void ChangeSphereTexture(const ETextureType Type)override;

	//モデルのアルファ値を設定する
	void SetAlpha(const float Alpha);

	//モデルがライトの影響を受けるか取得・設定
	virtual bool* IsEnableLight()override;
	virtual void SetEnableLight(const bool Enable)override;

	//深度を無効にするか取得・設定
	bool* IsDisableDepth() { return&m_IsDisableDepth; }
	void SetDisableDepth(const bool Type) { m_IsDisableDepth = Type; }
	
	//それぞれのコリジョンの種類を設定・取得
	void SetPlayerCollisionTag(const ECollisionTagName CollisionTag) { m_PlayerCollisionTag = CollisionTag; }
	ECollisionTagName GetPlayerCollisionTag() const { return m_PlayerCollisionTag; }
	void SetObjectCollisionTag(const ECollisionTagName CollisionTag) { m_ObjectCollisionTag = CollisionTag; }
	ECollisionTagName GetObjectCollisionTag() const { return m_ObjectCollisionTag; }
	void SetCameraCollisionTag(const ECollisionTagCameraName CollisionTag) { m_CameraCollisionTag = CollisionTag; }
	ECollisionTagCameraName GetCameraCollisionTag() const { return m_CameraCollisionTag; }
	void SetStepsCollisionTag(const ECollisionTagSteps CollisionTag) { m_StepsCollisionTag = CollisionTag; }
	ECollisionTagSteps GetStepsCollisionTag() const { return m_StepsCollisionTag; }
};
