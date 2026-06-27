#pragma once
//影用の描画を行うクラス
#include "../Renderer/ShaderStruct.h"
#include "../Calculation/Vector.h"
#include <vector>
using namespace FrameWork;
using namespace DirectX;

enum class EShadowType;

class CShadowMap
{
private:
	//影に使用するライト情報
	struct ShadowLightData
	{
		Vector3 Position;
		Vector3 Rotation;
		XMMATRIX World;
	};
	//CBufferに渡すライトに必要な情報
	struct CBufferLightData
	{
		XMFLOAT4 LightPos;//ライト位置
		XMFLOAT4 Eye;//カメラ位置
		XMFLOAT4 ShadowView;//影の許容値 Xのみ使用
	};
	//CBufferに渡すスタティックメッシュ情報
	struct BufferStaticMeshData
	{
		XMMATRIX W;//ワールド行列
		XMMATRIX WVP;//ワールドから射影までの変換行列
		XMMATRIX WLP;//ワールド・”ライトビュー”・プロジェクションの合成
		XMMATRIX WLPT;//ワールド・”ライトビュー”・プロジェクション・テクスチャ座標行列の合成
		XMFLOAT4 Front;
	};
	//CBufferに渡すスケルタルメッシュ情報
	struct BufferSkeletalMeshData
	{
		XMMATRIX W;//ワールド行列
		XMMATRIX WVP;//ワールドから射影までの変換行列
		XMMATRIX WLP;//ワールド・”ライトビュー”・プロジェクションの合成
		XMMATRIX WLPT;//ワールド・”ライトビュー”・プロジェクション・テクスチャ座標行列の合成
		XMMATRIX View;
		XMMATRIX Projection;
		XMMATRIX LightView;
		XMMATRIX ShaderShadowLightProjection;
		XMFLOAT4 Front;
		XMFLOAT4 FogParam;
	};

	//ライト情報
	static ShadowLightData m_LightData;
	static Vector3 m_DefaultPosition;
	static Vector3 m_DefaultRotation;
	static Vector3 m_LightFront;
	//ライトのマトリクス
	static XMMATRIX m_LightProjectionMatrix;
	static XMMATRIX m_LightView;
	//真下の影用変数
	static Vector3 m_LightFrontUP;
	static XMMATRIX m_LightViewUp;
	static XMMATRIX m_LightProjectionUp;
	static ShaderShadowLightProjection m_ShaderLightProjection;
	static ShaderShadowLightProjection m_DefaultShaderLightProjection;

	//シェーダ用バッファー
	//スタティックメッシュ用バッファー
	static ID3D11InputLayout* m_VertexLayout;
	static ID3D11Buffer* m_LightDataBuffer;
	static ID3D11Buffer* m_StaticMeshBuffer;
	static ID3D11Buffer* m_MaterialBuffer;
	static ID3D11Buffer* m_MatrixAndFogBuffer;
	static ID3D11Buffer* m_DirectionalLightBuffer;
	static ID3D11VertexShader* m_NormalMapVertexShader;//ノーマルマップレンダリング用
	static ID3D11PixelShader* m_NormalMapPixelShader;//ノーマルマップレンダリング用
	static ID3D11VertexShader* m_VertexShader;//通常レンダリング用
	static ID3D11PixelShader* m_PixelShader;//通常レンダリング用
	static ID3D11VertexShader* m_LightVertexShader;//通常レンダリング用（ライトあり）
	static ID3D11PixelShader* m_LightPixelShader;//通常レンダリング用（ライトあり）
	static ID3D11VertexShader* m_DisableSelfLightVertexShader;//通常レンダリング用（ライトあり,セルフシャドウなし）
	static ID3D11PixelShader* m_DisableSelfLightPixelShader;//通常レンダリング用（ライトあり）
	static ID3D11VertexShader* m_DepthVertexShader;//深度テクスチャーレンダリング用
	static ID3D11PixelShader* m_DepthPixelShader;//深度テクスチャーレンダリング用　　

	//スケルタルメッシュ用バッファー
	static ID3D11InputLayout* m_SkeletalVertexLayout;
	static ID3D11Buffer* m_SkeletalLightBuffer;
	static ID3D11Buffer* m_SkeletalMeshBuffer;
	static ID3D11Buffer* m_SkeletalMaterialBuffer;
	static ID3D11Buffer* m_SkeletalBoneDataBuffer;
	static ID3D11VertexShader* m_SkeletalVertexShader;//通常レンダリング用
	static ID3D11PixelShader* m_SkeletalPixelShader;//通常レンダリング用
	static ID3D11VertexShader* m_SkeletalDepthVertexShader;//深度テクスチャーレンダリング用
	static ID3D11PixelShader* m_SkeletalDepthPixelShader;//深度テクスチャーレンダリング用　

	static XMMATRIX m_ClipToUV;//テクスチャ用行列

	static float m_ShadowView;//影の許容値

	static void StaticMeshShaderInit();
	static void SkinMeshShaderInit();

public:
	static void Init();
	static void UnInit();
	static void Update();
	static void SetConstantBuffers();
	static void SetSkinConstantBuffers();
	//シェーダにカメラ位置とライト位置を送る
	static void SetLightCameraPosition();
	static void SetSkinLightCameraPosition();
	//影用のデプスシェーダーセット
	static void SetDepthShader();
	static void SetSkinDepthShader();
	//ノーマルモデル描画用シェーダーセット
	static void SetNormalModelShader();
	static void SetNormalModelLightShader();
	static void SetNormalModelDisableSelfLightShader();
	static void SetNormalMappingModelShader();
	
	//スキンメッシュ描画用シェーダーセット
	static void SetSkinMeshShader();
	//ノーマルモデルのマトリクスをセット
	static void SetNormalModelMatrix(const EShadowType Type, const XMMATRIX& Matrix);
	//スキンメッシュのマトリクスをセット
	static void SetSkinMeshMatrix(const EShadowType Type, const UINT NumBone, const std::vector<BoneNameMatrixData>& m_BoneList, const XMMATRIX& Matrix);
	//ノーマルモデルのマテリアルをセット
	static void SetNormalModelMaterial(const XMFLOAT4& Ambient, const XMFLOAT4& Diffuse, const XMFLOAT4& Specular, const XMFLOAT4& Alpha);
	//ノーマルモデルのマトリクスとカメラ情報とライト情報セット
	static void SetNormalModelCameraLight(const XMMATRIX& Matrix);
	//スキンメッシュのマテリアルをセット
	static void SetSkinMeshMaterial(const XMFLOAT4& Ambient, const XMFLOAT4& Diffuse, const XMFLOAT4& Specular, const XMFLOAT4& Alpha);
	//デプス用ビューポート設定
	static void SetDepthViewPort();
	//通常描画ビューポート設定
	static void SetNormalViewPort();

	static Vector3 GetLightRotation() { return m_LightData.Rotation; }
	static ShaderShadowLightProjection GetLightProjection() { return m_ShaderLightProjection; }

	static void SetLightRotation(Vector3 rot) { m_LightData.Rotation = rot; }
	static void SetLightProjection(const ShaderShadowLightProjection& Projection) { m_ShaderLightProjection = Projection; }

	static void LightRotationReset() { m_LightData.Rotation = m_DefaultRotation; }
	static void LightProjectionReset() { m_ShaderLightProjection = m_DefaultShaderLightProjection; }
	static float* GetShadowView() { return &m_ShadowView; }
};