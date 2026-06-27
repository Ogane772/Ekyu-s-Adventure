//グローバル
Texture2D g_texDecal: register(t0);
Texture2D g_texDepth : register(t1);//深度テクスチャー
Texture2D g_texNormal : register(t2);
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

cbuffer NormalBuffer : register(b5)
{
	float4 SpecularPower;//xのみ使用 スペキュラの鋭さ
	float4 NormalPower;//xのみ使用　法線マップの強さ
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

	float4 Binormal : BINORMAL0;
	float4 Tangent : TANGENT0;
	float4 Diffuse : COLOR0;
	float4 LightDirection : POSITION2;
	float4 CameraPosition : POSITION3;
	float4 Specular : COLOR1;
	float4 NormalPower : NORMAL1;
};
//
//バーテックスシェーダー
//

VS_OUTPUT VS(in float4 inPosition : POSITION0,   
	in float4 inNormal : NORMAL0,
	in float4 inBinormal : BINORMAL0,
	in float4 inTangent : TANGENT0,
	in float4 inDiffuse : COLOR0,
	in float2 inTexCoord : TEXCOORD0)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	//射影変換（ワールド→ビュー→プロジェクション）
	output.Pos = mul(inPosition, g_mWVP);

	inNormal = normalize(inNormal);
	inNormal.w = 0.0;

	//法線をモデルの姿勢に合わせる(モデルが回転すれば法線も回転させる必要があるため）
	output.Normal = normalize(mul(inNormal, (float3x3)g_mW));

	//ライト方向
	output.Light = normalize(Light.Direction);
	//視線ベクトル　ワールド空間上での頂点から視点へ向かうベクトル
	output.PosWorld = mul(inPosition, g_mW);
	output.EyeVector = normalize(g_vEye - output.PosWorld);
	//テクスチャー座標
	output.Tex = inTexCoord;
	//ライトビューを参照するとき、手がかりとなるテクスチャー座標
	output.LightTexCoord = mul(inPosition, g_mWLPT);//この点が、ライトビューであったときの位置がわかる
											 //ライトビューにおける位置(変換後)

	output.LighViewPos = mul(inPosition, g_mWLP);


	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	inBinormal = normalize(inBinormal);
	inBinormal.w = 0.0;
	output.Binormal = mul(inBinormal, World);

	inTangent = normalize(inTangent);
	inTangent.w = 0.0;
	output.Tangent = mul(inTangent, World);

	output.Diffuse = inDiffuse;

	output.LightDirection = Light.Direction;
	output.CameraPosition = CameraPos;

	output.Specular = SpecularPower;
	output.NormalPower = NormalPower;

	return output;
}

//
//ピクセルシェーダー
//
float4 PS(VS_OUTPUT input) : SV_Target
{

	float4 color;

	color = g_texDecal.Sample(g_samLinear, input.Tex);
	//影の処理
	float2 LightTexCoord2 = input.LighViewPos.xy / input.LighViewPos.w;
	LightTexCoord2.x = 0.5f + (0.5f * LightTexCoord2.x);
	LightTexCoord2.y = 0.5f - (0.5f * LightTexCoord2.y);
	//LightTexCoord2 /= LightTexCoord2.w;
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

	float3 eyev = input.PosWorld - input.CameraPosition; // 視線ベクトル
	eyev = normalize(eyev);

	// 法線マッピング
	float4 normalMap = g_texNormal.Sample(g_samLinear, input.Tex);
	normalMap = (normalMap * input.NormalPower.x) - 1.0;
	normalMap.xyz = (normalMap.x * input.Binormal) + (-normalMap.y * input.Tangent) + (normalMap.z * input.Normal);
	normalMap.w = 0.0;
	normalMap = normalize(normalMap);

	float3 refv = reflect(eyev.xyz, normalMap.xyz); // 反射させたいベクトル, 面の法線
	refv = normalize(refv);

	float light = -dot(normalMap.xyz, eyev.xyz);  // ランバート

	light = saturate(light);

	color = input.Diffuse;
	color *= g_texDecal.Sample(g_samLinear, input.Tex);
	color *= light;

	// スぺキュラ
	float specular = -dot(eyev, refv);
	specular = saturate(specular);
	specular = pow(specular, input.Specular.x); // 乗数でスぺキュラの鋭さが変わる(大きいほど輝かない）

	color += specular;

	color.a = 1.0f;

	color.w = g_Alpha.x;

	return color;
}