#include "shaderfunc.hlsl"
Texture2D ColorTexture :register(t0);
SamplerState Sampler : register(s0);
//*****************************************************************************
// 定数バッファ
//*****************************************************************************
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
void main(in float4 inPosition : SV_POSITION,
	in float4 inWorldPosition : POSITION1,
	in float4 inNormal : NORMAL0,
	in float4 inBinormal : BINORMAL0,
	in float4 inTangent : TANGENT0,
	in float4 inDiffuse : COLOR0,
	in float4 inAmbient : COLOR2,
	in float2 inTexCoord : TEXCOORD0,
	in float4 inLightDirection : POSITION2,
	in float4 inCameraPosition : POSITION3,
	in float4 inSpecular : COLOR1,
	in float4 inNormalPower : NORMAL1,
	in float4 inAlpha : COLOR3,

	out float4 outDiffuse : SV_Target :SV_Target)
{
	outDiffuse = ColorTexture.Sample(Sampler, inTexCoord);
    outDiffuse.w = Dithering(ColorTexture, Sampler, inTexCoord, inAlpha.x, inPosition);
}
