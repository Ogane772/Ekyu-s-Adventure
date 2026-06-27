cbuffer ConstantBuffer:register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 CameraPos;
}

struct LIGHT
{
	float4 Direction;
	float4 Diffuse;
	float4 Ambient;
};

cbuffer LightBuffer:register(b1)
{
	LIGHT Light;
}

cbuffer NormalBuffer : register(b2)
{
	float4 SpecularPower;//xのみ使用 スペキュラの鋭さ
	float4 NormalPower;//xのみ使用　法線マップの強さ
}

cbuffer per_material : register(b3)
{
	float4 g_Ambient;//環境光
	float4 g_Diffuse; //拡散反射光
	float4 g_Specular;//鏡面反射光
	float4 g_Alpha;//透明度
};

void main(in float4 inPosition         : POSITION0,   // :セマンティクス(変数の役割を決める)
		  in float4 inNormal           : NORMAL0,
		  in float4 inBinormal         : BINORMAL0,
		  in float4 inTangent          : TANGENT0,
		  in float4 inDiffuse          : COLOR0,
		  in float2 inTexCoord         : TEXCOORD0,

	out float4 outPosition : SV_POSITION,
	out float4 outWorldPosition : POSITION1,
	out float4 outNormal : NORMAL0,
	out float4 outBinormal : BINORMAL0,
	out float4 outTangent : TANGENT0,
	out float4 outDiffuse : COLOR0,
	out float4 outAmbient : COLOR2,
	out float2 outTexCoord : TEXCOORD0,
	out float4 outLightDirection : POSITION2,
	out float4 outCameraPosition : POSITION3,
	out float4 outSpecular : COLOR1,
	out float4 outNormalPower : NORMAL1,
	out float4 outAlpha : COLOR3
)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	outPosition = mul(inPosition, wvp);

	outWorldPosition = mul(inPosition, World);
	inNormal = normalize(inNormal);
	inNormal.w = 0.0;
	outNormal = mul(inNormal, World);

	inBinormal = normalize(inBinormal);
	inBinormal.w = 0.0;
	outBinormal = mul(inBinormal, World);

	inTangent = normalize(inTangent);
	inTangent.w = 0.0;
	outTangent = mul(inTangent, World);

	outDiffuse = g_Diffuse;
	outAmbient = g_Ambient;
    outAlpha = g_Alpha;
	outTexCoord = inTexCoord;

	outLightDirection = Light.Direction;
	outCameraPosition = CameraPos;

	outSpecular = SpecularPower;
	outNormalPower = NormalPower;
}