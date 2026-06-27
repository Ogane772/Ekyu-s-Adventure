#pragma once
#include <d3d11.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/Importer.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "StaticMeshType.h"
#include "../Renderer/ShaderStruct.h"
#include "../Calculation/Vector.h"
#include "../Calculation/Matrix.h"
#include "../Collision/CollisionTag.h"
using namespace FrameWork;
class CSkyShader;
enum class ETextureType;
enum class EShadowType;

class CStaticMesh
{
private:
	struct ShaderVertexData
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 Binormal;
		XMFLOAT3 Tangent;
		XMFLOAT4 Diffuse;
		XMFLOAT2 TexCoord;
	};
	struct StaticMeshData
	{
		ID3D11Buffer* VertexBuffer = nullptr;
		ID3D11Buffer* IndexBuffer = nullptr;
		UINT IndexNum = 0;
		std::weak_ptr<ID3D11ShaderResourceView> Texture;
		std::weak_ptr<ID3D11ShaderResourceView> NormalTexture;
		EStaticMeshType ModelIndex;
		const aiScene* Scene = nullptr;
		std::vector<std::vector<Vector3>> TriangleList;
		UINT MeshNum = 0;
	};

	struct TEX_SCROLL
	{
		XMFLOAT4 TexScroll;//テクスチャスクロール用変数 X,Yのみ使用
	};

	//スカイドーム用のシェーダークラス
	CSkyShader* m_SkyShader = nullptr;
	
	std::unordered_map<std::string, aiQuaternion> m_NodeRotation;
	std::unordered_map<std::string, aiVector3D> m_NodePosition;
	//モデルの頂点情報
	std::vector<std::vector<Vector3>> m_TriangleList;
	//コリジョンに使用する頂点情報
	std::vector<CollisionMesh> m_CollisionMeshList;
	StaticMeshData* m_Mesh = nullptr;//モデルのメッシュ情報
	
	//ノーマルマップ情報
	ShaderNormalMapData m_NormalMapPower;
	EStaticMeshType m_MeshType;
	bool m_IsLoadNormalMap = false;//ノーマルマップを読み込んでいるか
	bool m_IsEnableLight = false;//ライトが有効か
	bool m_IsDisableSelfShadow = false;//影を無効化するか

	//階層構造モデル数
	UINT m_MeshNum = 0;
	const aiScene* m_Scene = nullptr;
	
	MATERIAL m_Material;
	XMMATRIX m_World = XMMATRIX();
	XMFLOAT4 m_TexScroll = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	// モデルのファイルパス情報
	static std::unordered_map<EStaticMeshType, std::string> m_ModelFileList;
	//一度読み込んだモデルは使いまわすため読み込んだモデル情報を格納
	static std::vector<StaticMeshData*> m_LoadMeshList;

	//シェーダーに使用する頂点レイアウトや定数バッファー等は全モデルで共有するためstaticにする
	static ID3D11InputLayout* m_VertexLayout;
	static ID3D11Buffer* m_MatrixAndFogConstantBuffer;
	static ID3D11Buffer* m_LightConstantBuffer;
	static ID3D11Buffer* m_NormalMapConstantBuffer;
	static ID3D11Buffer* m_MaterialConstantBuffer;
	static ID3D11Buffer* m_TexScrollConstantBuffer;
	static ID3D11VertexShader* m_VertexShader;
	static ID3D11PixelShader* m_PixelShader;
	static ID3D11VertexShader* m_NoShadowVertexShader;
	static ID3D11PixelShader* m_NoShadowPixelShader;
	static ID3D11PixelShader* m_NoNormalPixelShader;

	void ConvertAssimpMatrix(Matrix4& OutMatrix, const aiMatrix4x4 & AiMatrix);
	void Load(const std::string& FileName);
	void LoadMaterial(const aiMaterial* AiMaterial, const int Index, const std::string& FileName);
	void UpdateMatrix(const aiNode* Node, const XMMATRIX Matrix);
	void DrawMesh(const aiNode* Node, const XMMATRIX Matrix);
	void DrawShadowMesh(const EShadowType Type, const aiNode* Node, const XMMATRIX Matrix);
	void DrawShadowDepth(const EShadowType Type, const aiNode* Node, const  XMMATRIX Matrix);

public:

	//モデルの読み込みを行う
	CStaticMesh(const EStaticMeshType Type);
	CStaticMesh(const std::string& FileName);

	CStaticMesh() {};
	~CStaticMesh();

	//ロード済みのモデルを全て解放する
	static void AllLoadModelDelete();
	//1つだけ生成されていれば良いシェーダー用のデータやロードするモデルのファイル名等を初期化する
	static void InitModelData();
	static void UnInitModelData();
	//指定したモデルのファイル名を返す
	static const std::string& GetModelFileName(const EStaticMeshType Type) { return m_ModelFileList[Type]; }

	void SetShader(const int Index, const XMMATRIX& World);
	void Update(const XMMATRIX Matrix);
	void Draw(const XMMATRIX Matrix);
	void ShadowOnModelDraw(const EShadowType Type, const XMMATRIX Matrix);
	void ShadowDraw(const EShadowType Type, const XMMATRIX Matrix);
	//コリジョンメッシュ用の頂点情報作成
	void CreateCollisionMesh();

	//ノーマルマップを読み込んでいるか返す
	bool IsLoadNormalMap() const { return m_IsLoadNormalMap; }
	ShaderNormalMapData* GetNormalMapPower() { return& m_NormalMapPower; }
	std::vector<std::vector<Vector3>>* GetTriangleList() { return &m_TriangleList; }
	std::vector<std::vector<Vector3>>* GetMeshTriangleList() { return &m_Mesh->TriangleList; }
	//モデルのメッシュコリジョンを渡す
	std::vector<CollisionMesh>* GetNormalModelMeshCollision();

	//エディタ等で使用する各種セッター/ゲッター
	EStaticMeshType GetMeshType() const { return m_MeshType; }
	const XMMATRIX& GetWorld() { return m_World; }
	MATERIAL* GetMaterial() { return &m_Material; }
	void SetNormalMapPower(ShaderNormalMapData cons) { m_NormalMapPower = cons; }
	void ChangeTexture(const ETextureType Type, const int Index);
	void SetAlpha(const float Alpha);
	bool* IsEnableLight();
	void SetEnableLight(const bool Enable);
	bool* IsDisableSelfShadow();
	void SetDisableSelfShadow(const bool type);
	bool* IsEnableNormalMap();
	void SetEnableNormalMap(const bool Enable);
	void SetMaterial(const MATERIAL& Material);
};