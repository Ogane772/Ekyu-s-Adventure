
//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マテリアルバッファ
cbuffer MaterialBuffer : register(b1)
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float4		Alpha;
	float		Shininess;
	float3		Dummy;//16bit境界用
}

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);

struct PSSkinIn
{
	float4 Pos	: SV_Position;//位置
	float3 Normal : NORMAL;//頂点法線
	float2 Tex	: TEXCOORD;//テクスチャー座標
	float4 Color : COLOR0;//最終カラー（頂点シェーダーにおいての）
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(PSSkinIn input) :SV_Target
{
	float4 outcolor;
	outcolor = g_Texture.Sample(g_SamplerState, input.Tex);

	outcolor *= input.Color;

	return outcolor;
}