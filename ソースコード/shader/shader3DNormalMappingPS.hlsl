Texture2D ColorTexture :register(t0);
Texture2D NormalTexture :register(t2);
SamplerState Sampler : register(s0);

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

	out float4 outDiffuse : SV_Target)   // 色
{
	float3 eyev = inWorldPosition - inCameraPosition; // 視線ベクトル
	eyev = normalize(eyev);

	// 法線マッピング
	float4 normalMap = NormalTexture.Sample(Sampler, inTexCoord);
	normalMap = (normalMap * inNormalPower.x) - 1.0;
	normalMap = (normalMap.x * inBinormal) + (-normalMap.y * inTangent) + (normalMap.z * inNormal);
	normalMap.w = 0.0;
	normalMap = normalize(normalMap);

	float3 refv = reflect(eyev.xyz, normalMap.xyz); // 反射させたいベクトル, 面の法線
	refv = normalize(refv);

	float light = -dot(normalMap.xyz, -inLightDirection.xyz);  // ランバート

	light = saturate(light);

	float4 ambient = inAmbient;
	outDiffuse = ambient + inDiffuse;
	outDiffuse *= ColorTexture.Sample(Sampler, inTexCoord);

	// スぺキュラ
	float specular = -dot(inLightDirection.xyz, refv);
	specular = saturate(specular);
	specular = pow(specular, inSpecular.x); // 乗数でスぺキュラの鋭さが変わる(大きいほど輝かない）

	outDiffuse += specular;

	outDiffuse.a = 1.0f;
}