#pragma once
//ボーンがあるモデルクラス
#include <d3d11.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/Importer.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "SkeletalMeshType.h"
#include "../Renderer/ShaderStruct.h"

enum class EShadowType;

class CSkeletalMesh
{
private:
	// スケルタルメッシュ頂点構造体
	struct ShaderVertexData
	{
		XMFLOAT3 Position = XMFLOAT3();
		XMFLOAT3 Normal = XMFLOAT3();
		XMFLOAT2 TexCoord = XMFLOAT2();
		UINT BoneIndex[4] = { 0 ,0 ,0 ,0 };//最大4つまでボーンの影響を計算できるので4個まで
		float BoneWeight[4] = { 0.0f ,0.0f ,0.0f ,0.0f };
	};
	//シェーダーに渡すボーンのウェイト情報
	struct ShaderWeightData
	{
		int Index = 0;
		float Weight = 0.0f;
	};
	//シェーダーに渡すカメラと行列情報
	struct ShaderSkeletalMatrix
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
		XMFLOAT4 Eye;//カメラ位置
	};
	//シェーダーに渡す頂点、テクスチャ情報
	struct ShaderMeshData
	{
		ID3D11Buffer* VertexBuffer = nullptr;
		ID3D11Buffer* IndexBuffer = nullptr;
		UINT IndexNum = 0;
		std::weak_ptr<ID3D11ShaderResourceView> Texture;
	};
	//ボーンの行列情報
	struct BoneMatrix
	{
		aiMatrix4x4 Matrix;
		aiMatrix4x4 AnimationMatrix;
		aiMatrix4x4 OffsetMatrix;
	};
	struct AnimationData
	{
		float Time = 0.0;        // 現在のアニメ時刻（ticks）
		float PrevTime = 0.0;    // 前のアニメ時刻（ブレンド時に参照）
		float AnimationSpeed = 1.0f;//アニメーションスピード（倍率）
		int CurrentAnimeNumber = 0;//現在再生しているアニメ番号を格納
		int PrevAnimeNumber = 0;//一つ前のアニメ番号を格納
		float BlendTime = 0.0f;//補間時間　BLEND_END_TIMEを超えたらブレンドを終了する
		bool IsLoop = false;//ループするか
		bool IsEnd = false;//非ループ時にアニメーションが終了したか
	};

	// モデルのファイルパス情報
	static std::unordered_map<ESkeletalMeshType, std::string> m_ModelFileList;
	//シェーダーに使用する頂点レイアウトや定数バッファー等は全モデルで共有するためstaticにする
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11InputLayout* m_VertexLayout;
	static ID3D11Buffer* m_LightConstantBuffer;
	static ID3D11Buffer* m_MatrixConstantBuffer;
	static ID3D11Buffer* m_BoneConstantBuffer;
	static ID3D11Buffer* m_MaterialConstantBuffer;

	MATERIAL m_Material;
	aiAnimation* m_AiAnimation = nullptr;
	AnimationData m_AnimationData;
	ShaderMeshData* m_Mesh = nullptr;//動的配列ポインター
	const aiScene* m_Scene = nullptr;
	std::vector<BoneNameMatrixData> m_BoneList;
	std::unordered_map<std::string, BoneMatrix> m_Bone;

	void Load(const std::string& FileName);
	//一致するボーンの名前番号をm_BoneListに格納
	int BoneNameIndex(const std::string& Name);
	//m_BoneListに情報を格納
	void CreateBone(aiNode* Node);
	//m_Boneに情報を格納
	void CreateBoneNormal(aiNode* Node);
	void DrawMesh(const aiNode* Node, const XMMATRIX Matrix);
	void DrawShadowMesh(const EShadowType Type, const aiNode* Node, const XMMATRIX Matrix);
	void UpdateBoneMatrix(const aiNode* Node, const aiMatrix4x4 Matrix);
	// 補間なしのアニメーション更新
	void UpdateAnimation(const float AnimationTime);
	// 補間ありのアニメーション更新
	void UpdateAnimationBlend(const int PrevAnime, const int CurrentAnime, const float BlendTime, const float AnimationTime);
	//現在のアニメーション再生時間から対応するモーションの移動/回転/大きさを取得
	void SamplingPosition(const aiNodeAnim* Channel, const float AnimationTime, aiVector3D& Out);
	void SamplingRotation(const aiNodeAnim* Channel, const float AnimationTime, aiQuaternion& Out);
	void SamplingScaling(const aiNodeAnim* Channel, const float AnimationTime, aiVector3D& Out);

public:
	static const int BONEMODEL_FILE_COUNT;
	//スキンメッシュシェーダーをセット
	static void ShaderInit();
	static void ShaderUnInit();

	CSkeletalMesh(const ESkeletalMeshType Type);
	CSkeletalMesh(const std::string& FileName);
	~CSkeletalMesh();
	void Update(float DeltaTime);
	void Draw(const XMMATRIX Matrix);
	void ShadowOnModelDraw(const EShadowType Type, const XMMATRIX Matrix);
	void ShadowDraw(const EShadowType Type, const XMMATRIX Matrix);
	int SetAnimationChange(const int AnimationNumber);
	int SetAnimationChange(const bool IsLoop, const int AnimationNumber, const bool IsBlend, const float AnimationSpeed);
	void SetAnimationSpeed(const float AnimationSpeed) { m_AnimationData.AnimationSpeed = AnimationSpeed; }

	aiAnimation* GetAiAnimation() const { return m_AiAnimation; }
	float GetAnimationSpeed() const { return m_AnimationData.AnimationSpeed; }
	const aiScene* GetAiScene() const { return m_Scene; }
	float GetFrame() const { return m_AnimationData.Time; }
	bool IsLoop() const { return m_AnimationData.IsLoop; }
	bool IsEnd() const { return m_AnimationData.IsEnd; }
	void SetLoop(const bool IsLoop) { m_AnimationData.IsLoop = IsLoop; }

	void ResetAnimeFrame() { m_AnimationData.Time = 0.0f; }
};