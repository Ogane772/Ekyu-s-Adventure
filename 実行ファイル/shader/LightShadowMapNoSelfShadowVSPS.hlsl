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
	output.Pos = mul(Pos, g_mWVP);
	//法線をモデルの姿勢に合わせる(モデルが回転すれば法線も回転させる必要があるため）
	output.Normal = normalize(mul(Norm, (float3x3)g_mW));

	//ライト方向
	output.Light = Light.Direction;
	//視線ベクトル　ワールド空間上での頂点から視点へ向かうベクトル
	output.PosWorld = mul(Pos, g_mW);
	output.EyeVector = normalize(g_vEye - output.PosWorld);
	//テクスチャー座標
	output.Tex = Tex;
	//ライトビューを参照するとき、手がかりとなるテクスチャー座標
	output.LightTexCoord = mul(Pos, g_mWLPT);//この点が、ライトビューであったときの位置がわかる
											 //ライトビューにおける位置(変換後)

	output.LighViewPos = mul(Pos, g_mWLP);

	return output;
}

//
//ピクセルシェーダー
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	//環境光　項
	float4 ambient = g_Ambient;
	//拡散反射光　項
	float NL = saturate(dot(input.Normal, input.Light));
	float4 diffuse = (g_Diffuse / 2 + g_texDecal.Sample(g_samLinear, input.Tex, -1.0) / 2)*NL;

	//鏡面反射光　項
	float3 reflect = normalize(2 * NL*input.Normal - input.Light);
	float4 specular = pow(saturate(dot(reflect,input.EyeVector)), 4)*g_Specular;
	//フォンモデル最終色　３つの項の合計
	float4 color = ambient + diffuse + specular;
	color *= g_texDecal.Sample(g_samLinear, input.Tex);

	color.w = g_texDecal.Sample(g_samLinear, input.Tex).w;
	color.w = g_Alpha.x;
	return color;
}