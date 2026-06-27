cbuffer ConstantBuffer:register(b0)
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

cbuffer LightBuffer:register(b1)
{
	LIGHT Light;
}

cbuffer per_material : register(b2)
{
	float4 g_Ambient;//環境光
	float4 g_Diffuse; //拡散反射光
	float4 g_Specular;//鏡面反射光
	float4 g_Alpha;//透明度
};

cbuffer TexScroll : register(b3)
{
	float4 g_TexScroll;
};

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
//
//バーテックスシェーダー
//

VS_OUTPUT main(in float4 inPosition         : POSITION0,   // :セマンティクス(変数の役割を決める)
	in float4 inNormal : NORMAL0,
	in float4 inBinormal : BINORMAL0,
	in float4 inTangent : TANGENT0,
	in float4 inDiffuse : COLOR0,
	in float2 inTexCoord : TEXCOORD0)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	//射影変換（ワールド→ビュー→プロジェクション）
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	output.Pos = mul(inPosition, wvp);
	//法線をモデルの姿勢に合わせる(モデルが回転すれば法線も回転させる必要があるため）
	output.Normal = normalize(mul(inNormal, (float3x3)World));

	//ライト方向
	output.Light = Light.Direction;
	//視線ベクトル　ワールド空間上での頂点から視点へ向かうベクトル
	output.PosWorld = mul(inPosition, World);
	output.EyeVector = normalize(CameraPos - output.PosWorld);
	//テクスチャー座標
	inTexCoord.x = inTexCoord.x + g_TexScroll.x;
	inTexCoord.y = inTexCoord.y + g_TexScroll.y;
	output.Tex = inTexCoord;
	output.Ambient = g_Ambient;
	output.Diffuse = g_Diffuse;
	output.Specular = g_Specular;
	output.Alpha = g_Alpha;

	return output;
}