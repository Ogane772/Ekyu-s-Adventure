#include "shaderfunc.hlsl"
//グローバル
Texture2D g_texDecal: register(t0);
Texture2D g_texDepth : register(t1);//深度テクスチャー
SamplerState g_samLinear : register(s0);

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
	float4 g_LightFront;
};

cbuffer per_material:register(b2)
{
	float4 g_Ambient;//環境光
	float4 g_Diffuse; //拡散反射光
	float4 g_Specular;//鏡面反射光
	float4 g_Alpha;//透明度
};

cbuffer ConstantBuffer:register(b3)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 CameraPos;
	float4 FogParam;
}

struct LIGHT
{
	float4 Direction;
	float4 Diffuse;
	float4 Ambient;
};

cbuffer LightBuffer:register(b4)
{
	LIGHT Light;
}

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
VS_OUTPUT_DEPTH VS_Depth( float4 Pos : POSITION)
{
	VS_OUTPUT_DEPTH output = (VS_OUTPUT_DEPTH)0;
	output.Pos=mul(Pos,g_mWLP);
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

VS_OUTPUT VS(in float4 Pos         : POSITION,   // :セマンティクス(変数の役割を決める)
	in float4 Norm : NORMAL,
	in float4 inBinormal : BINORMAL0,
	in float4 inTangent : TANGENT0,
	in float4 inDiffuse : COLOR0,
	in float2 Tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	//射影変換（ワールド→ビュー→プロジェクション）
	output.Pos=mul(Pos,g_mWVP);
	//法線をモデルの姿勢に合わせる(モデルが回転すれば法線も回転させる必要があるため）
	output.Normal= normalize(mul(Norm, (float3x3)g_mW));
	
	//ライト方向
	output.Light=normalize(Light.Direction);
	//視線ベクトル　ワールド空間上での頂点から視点へ向かうベクトル
	output.PosWorld = mul(Pos,g_mW);
    output.EyeVector = normalize(g_vEye - output.PosWorld);
	//テクスチャー座標
	output.Tex=Tex;
	//ライトビューを参照するとき、手がかりとなるテクスチャー座標
	output.LightTexCoord=mul(Pos ,g_mWLPT);//この点が、ライトビューであったときの位置がわかる
	//ライトビューにおける位置(変換後)
	output.LighViewPos=mul(Pos,g_mWLP);

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
	color.w = g_Alpha.x;
	color.rgb = (FogParam.x == 1.0) ? FogColor(input.PosWorld, color, CameraPos, FogParam.y, FogParam.z) : color;
	return color;
}