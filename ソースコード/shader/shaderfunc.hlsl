// 4x4 Bayer matrix 
static const int g_Bayer4x4[16] =
{
    0, 8, 2, 10,
	12, 4, 14, 6,
	3, 11, 1, 9,
	15, 7, 13, 5
};

float Dithering(in Texture2D inTexture, in SamplerState inSampler, in float2 inTex, in float4 inAlpha, in float4 Pos)
{
    const float textureAlpha = inTexture.Sample(inSampler, inTex).w;
    const float alpha = textureAlpha * inAlpha.x;

	// ディザリング処理(Bayer 4x4 順序付きディザ)
    const int pixelX = (int) floor(Pos.x);
    const int pixelY = (int) floor(Pos.y);
    const int ix = pixelX & 3;
    const int iy = pixelY & 3;
    const int idx = iy * 4 + ix;
    const int bayer = g_Bayer4x4[idx];
    const float threshold = (bayer + 0.5f) / 16.0f;
    clip(alpha - threshold);
	
    return 1.0;//アルファ値を返す。
}

float3 FogColor(in float4 inPosition , in float4 InDiffuse, in float4 CameraPosition = 0.0,in float Near = 15.0,in float Far = 180.0)
{
	//線形フォグ計算
	const float d = distance(inPosition, CameraPosition);
	const float4 fogColor = 1.0;                  //フォグカラー
	const float  near = Near;             //フォグの開始位置
	const float  far = Far;             //フォグの終了位置

	float f = (far - d) / (far - near);
	f = clamp(f, 0.0f, 1.0f);
	//オブジェクトのランバート拡散照明の計算結果とフォグカラーを線形合成する
	InDiffuse = InDiffuse * f + fogColor * (1.0f - f);
	return InDiffuse;
}