#include "shaderNoise.hlsl"
#include "shaderfunc.hlsl"

Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);

// 定数バッファ
cbuffer ConstatntBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    float4 CameraPosition;
    float4 TextureScroll;
	float4 FogParam;
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main( in  float4 inPosition		: SV_POSITION,
            in float4 inWorldPosition   : POSITION0,
			in  float4 inNormal			: NORMAL0,
			in  float4 inDiffuse		: COLOR0,
			in  float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{

	//FMBの雲
    float cloud = fbm2(inTexCoord * 0.1, 20, TextureScroll.x);
	//雲の量
	cloud = cloud * 2.0 + 0.9;
	//saturate 0～1にする
	cloud = saturate(cloud);
	cloud = cloud * 0.8 + 0.05;
	cloud = cloud * cloud;
	
	//ルックアップテーブルを使い色を生成
	outDiffuse.rgb = g_Texture.Sample(g_SamplerState, float2(cloud, 0.0));

	outDiffuse.b *= 1.2;
	outDiffuse.a = 1.0;
	outDiffuse.rgb = (FogParam.x == 1.0) ? FogColor(inWorldPosition, outDiffuse, CameraPosition, FogParam.y, FogParam.z) : outDiffuse;
}
