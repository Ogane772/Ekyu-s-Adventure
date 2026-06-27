#include "shaderfunc.hlsl"
Texture2D g_texDecal: register(t0);
SamplerState g_samLinear : register(s0);
cbuffer ConstantBuffer:register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 CameraPos;
	float4 FogParam;
}

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Light : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float3 EyeVector : TEXCOORD2;
	float4 PosWorld : TEXCOORD3;
	float2 Tex : TEXCOORD4;
	float4 Ambient : COLOR1;
	float4 Diffuse : COLOR2;
	float4 Specular : COLOR3;
	float4 Alpha : COLOR4;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	//環境光　項
	float4 ambient = input.Ambient;
	//拡散反射光　項
	float NL = saturate(dot(input.Normal, input.Light));
	float4 diffuse = (input.Diffuse / 2 + g_texDecal.Sample(g_samLinear, input.Tex) / 2)*NL;

	//鏡面反射光　項
	float3 reflect = normalize(2 * NL*input.Normal - input.Light);
	float4 specular = pow(saturate(dot(reflect,input.EyeVector)), 4)*input.Specular;
	//フォンモデル最終色　３つの項の合計
	float4 color = ambient + diffuse + specular;
	color *= g_texDecal.Sample(g_samLinear, input.Tex);

	color.w = g_texDecal.Sample(g_samLinear, input.Tex).w;
	color.w = input.Alpha.x;
	color.rgb = (FogParam.x == 1.0) ? FogColor(input.PosWorld, color, CameraPos, FogParam.y, FogParam.z) : color;
	return color;
}