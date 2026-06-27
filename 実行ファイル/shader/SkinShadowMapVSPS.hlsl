#include "shaderfunc.hlsl"
//グローバル
Texture2D g_texDecal: register(t0);
Texture2D g_texDepth : register(t1);//深度テクスチャー
SamplerState g_samLinear : register(s0);
#define MAX_BONE_MATRICES 255

//グローバル
cbuffer per_frame:register(b0)
{
	float4 g_vLightPos;//ポイントライト
	float4 g_vEye;//カメラ位置
	float4 g_vShadowView;
};

cbuffer per_mesh:register(b1)
{
	matrix g_mW;//ワールド行列
	matrix g_mWVP; //ワールドから射影までの変換合成行列
	matrix g_mWLP; //ワールド・”ライトビュー”・プロジェクションの合成
	matrix g_mWLPT; //ワールド・”ライトビュー”・プロジェクション・UV行列の合成
	matrix View;
	matrix Projection;
	matrix LightView;
	matrix ShaderShadowLightProjection;
	float4 g_LightFront;
	float4 FogParam;
};

cbuffer per_material:register(b2)
{
	float4 g_Ambient;//環境光
	float4 g_Diffuse; //拡散反射光
	float4 g_Specular;//鏡面反射光
	float4 g_Alpha;//透明度
};

cbuffer global_bones:register(b3)//ボーンのポーズ行列が入る
{
	matrix g_mConstBoneWorld[MAX_BONE_MATRICES];
};
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
//バーテックスシェーダー出力構造体
struct VS_OUTPUT_DEPTH//深度テクスチャーレンダー用
{	
	float4 Pos : SV_POSITION;
	float4 Depth :POSITION;
};

struct VS_OUTPUT//通常レンダー用
{	
	float4 Pos : SV_POSITION;
	float3 Light : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float3 EyeVector : TEXCOORD2;
	float4 PosWorld : TEXCOORD3;
	float2 Tex : TEXCOORD4;
	float4 LightTexCoord : TEXCOORD5;
	float4 LighViewPos : TEXCOORD6;
};
//
//深度テクスチャー用バーテックスシェーダー
//
VS_OUTPUT_DEPTH VS_Depth(VSSkinIn input)
{
	matrix world_bone;

	world_bone = mul(g_mConstBoneWorld[input.Bones[0]], input.Weights[0]);
	world_bone += mul(g_mConstBoneWorld[input.Bones[1]], input.Weights[1]);
	world_bone += mul(g_mConstBoneWorld[input.Bones[2]], input.Weights[2]);
	world_bone += mul(g_mConstBoneWorld[input.Bones[3]], input.Weights[3]);
	matrix world_matrix = mul(world_bone, g_mW);
	
	matrix wvp;
	wvp = mul(world_matrix, LightView);
	wvp = mul(wvp, ShaderShadowLightProjection);

	float4 worldPos = mul(input.Pos, wvp);
	
	VS_OUTPUT_DEPTH output = (VS_OUTPUT_DEPTH)0;
	output.Pos = worldPos;
	output.Depth=output.Pos;

	return output;
}
//
//深度テクスチャー用ピクセルシェーダー
//
float4 PS_Depth( VS_OUTPUT_DEPTH input ) : SV_Target
{
	float4 color=input.Depth.z/input.Depth.w;
	color.w = 1.0f;
	return color;
}
//
//バーテックスシェーダー
//
VS_OUTPUT VS(VSSkinIn input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	matrix world_bone;

	world_bone = mul(g_mConstBoneWorld[input.Bones[0]], input.Weights[0]);
	world_bone += mul(g_mConstBoneWorld[input.Bones[1]], input.Weights[1]);
	world_bone += mul(g_mConstBoneWorld[input.Bones[2]], input.Weights[2]);
	world_bone += mul(g_mConstBoneWorld[input.Bones[3]], input.Weights[3]);

	matrix world_matrix;
	world_matrix = mul(world_bone, g_mW);
	matrix wvp;
	wvp = mul(world_matrix, View);
	wvp = mul(wvp, Projection);
	//射影変換（ワールド→ビュー→プロジェクション）
	output.Pos=mul(input.Pos,wvp);
	//法線をモデルの姿勢に合わせる(モデルが回転すれば法線も回転させる必要があるため）
	output.Normal= normalize(mul(input.Norm, (float3x3)world_matrix));

	matrix world_matrix2 = mul(world_bone, g_mW);

	matrix wvp2;
	wvp2 = mul(world_matrix2, LightView);
	wvp2= mul(wvp2, ShaderShadowLightProjection);

	float4 worldPos = mul(input.Pos, wvp2);

	//ライト方向
	output.Light=normalize(g_vLightPos);
	//視線ベクトル　ワールド空間上での頂点から視点へ向かうベクトル
	output.PosWorld = mul(input.Pos,g_mW);
    output.EyeVector = normalize(g_vEye - output.PosWorld);
	//テクスチャー座標
	output.Tex= input.Tex;
	//ライトビューを参照するとき、手がかりとなるテクスチャー座標
	output.LightTexCoord=mul(output.Pos,g_mWLPT);//この点が、ライトビューであったときの位置がわかる
	//ライトビューにおける位置(変換後)

	output.LighViewPos= worldPos;

	return output;
}

//
//ピクセルシェーダー
//
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float4 color = g_texDecal.Sample(g_samLinear, input.Tex);
	//影の処理
	float2 LightTexCoord2 = input.LighViewPos.xy / input.LighViewPos.w;
	LightTexCoord2.x = 0.5f + (0.5f * LightTexCoord2.x);
	LightTexCoord2.y = 0.5f - (0.5f * LightTexCoord2.y);

	//影がオブジェクトの近くにあるときは描画
	if (saturate(LightTexCoord2.x) == LightTexCoord2.x &&
		saturate(LightTexCoord2.y) == LightTexCoord2.y)
	{
		float TexValue = g_texDepth.Sample(g_samLinear, LightTexCoord2).r;
		float LightLength = input.LighViewPos.z / input.LighViewPos.w;
		//垂直の時は影を付けない
		if (dot(input.Normal, g_LightFront) != 0)
		{
			if (TexValue + g_vShadowView.x < LightLength)//ライトビューでの長さが短い（ライトビューでは遮蔽物がある）
			{
				color /= 3;//影
			}

		}
	}
	color.w = g_texDecal.Sample(g_samLinear, input.Tex).w;
	
	color.rgb = (FogParam.x == 1.0) ? FogColor(input.PosWorld, color, g_vEye, FogParam.y, FogParam.z) : color;
	return color;
}