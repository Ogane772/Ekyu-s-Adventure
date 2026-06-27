//定義
#define MAX_BONE_MATRICES 255

// ライトバッファ
struct LIGHT
{
	float4		Direction;
	float4		Diffuse;
	float4		Ambient;
};
cbuffer LightBuffer : register(b0)
{
	LIGHT		Light;
};

cbuffer global_1:register(b1)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 g_vEye;//カメラ位置
	float4 g_vLightPos;//ポイントライト情報（ライトの位置）
};

cbuffer global_bones:register(b2)//ボーンのポーズ行列が入る
{
	matrix g_mConstBoneWorld[MAX_BONE_MATRICES];
};

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	float3		Dummy;//16bit境界用
};

cbuffer MaterialBuffer : register(b3)
{
	MATERIAL	Material;
}

//バーテックスバッファーの入力
struct VSSkinIn
{//float3などの変数の後ろに数字があるやつは数を表す
 //float3 posはpos[3]と同じ
	float4 Pos	: POSITION;//位置   
	float3 Norm : NORMAL;//頂点法線
	float2 Tex	: TEXCOORD;//テクスチャー座標
	uint4  Bones : BONE_INDEX;//ボーンのインデックス
	float4 Weights : BONE_WEIGHT;//ボーンの重み
};
//ピクセルシェーダーの入力（バーテックスバッファーの出力）　
struct PSSkinIn
{
	float4 Pos	: SV_Position;//位置
	float3 Normal : NORMAL;//頂点法線
	float2 Tex	: TEXCOORD;//テクスチャー座標
	float4 Color : COLOR0;//最終カラー（頂点シェーダーにおいての）
	
};

//ライト用バーテックス
//バーテックスシェーダー
PSSkinIn main(VSSkinIn input)
{
	PSSkinIn output;
	
	matrix world_bone;

	world_bone = mul(g_mConstBoneWorld[input.Bones[0]], input.Weights[0]);
	world_bone += mul(g_mConstBoneWorld[input.Bones[1]], input.Weights[1]);
	world_bone += mul(g_mConstBoneWorld[input.Bones[2]], input.Weights[2]);
	world_bone += mul(g_mConstBoneWorld[input.Bones[3]], input.Weights[3]);

	matrix world_matrix;
	world_matrix = mul(world_bone, World);

	//射影変換（ワールド→ビュー→プロジェクション）
	matrix wvp;
	wvp = mul(world_matrix, View);
	wvp = mul(wvp, Projection);

	output.Pos = mul(input.Pos, wvp);
	//法線をモデルの姿勢に合わせる(モデルが回転すれば法線も回転させる必要があるため）
	output.Normal = normalize(mul(input.Norm, (float3x3)world_matrix));
	//テクスチャー座標
	output.Tex = input.Tex;

	//ディレクショナルライト計算

	float4 worldNormal, normal;
	normal = float4(input.Norm.xyz, 0.0);
	worldNormal = mul(normal, World);
	worldNormal = normalize(worldNormal);

	float light = 0.5 - 0.5 * dot(Light.Direction.xyz, worldNormal.xyz);

	output.Color = Material.Diffuse * light * Light.Diffuse;
	output.Color += Material.Ambient * (Light.Ambient * 2);
	output.Color.a = 1.0;
	output.Color.rgb = 1.0;
	return output;
}