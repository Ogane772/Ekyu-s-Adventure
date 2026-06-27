//乱数生成関数
//引数　シード値(2D)テクスチャ座標などを利用
//戻り値　疑似乱数(1D)
float random2(in float2 vec)
{//43758.5453123の所は大きい桁じゃないとだめ(sinの周期性を破壊するために大きな桁を入れている
	//sinはfloatのみなので内積でfloatに変換している
	return frac(sin(dot(vec.xy, float2(12.9898, 78.223)))*43758.5453123);
}
//2D疑似乱数
float2 random22(in float2 vec)
{
	vec = float2(dot(vec, float2(127.1, 311.7)),
		dot(vec, float2(269.5, 183.3)));
	//HLSLではsin(二次元ベクトル)を渡すとX,Yを別々で計算する
	return frac(sin(vec) * 43758.5453123);
}
//3D疑似乱数
float random3(in float3 vec)
{
	return frac(sin(dot(vec.xyz, float3(12.9898, 78.233, 47.2311))) * 43758.5453123);
}

float random33(in float3 vec)
{
	vec = float3(dot(vec, float3(127.1, 311.7, 245.4)),
		dot(vec, float3(269.5, 183.3, 131.2)),
		dot(vec, float3(522.3, 243.1, 532.4)));
	//HLSLではsin(二次元ベクトル)を渡すとX,Yを別々で計算する
	return frac(sin(vec) * 43758.5453123);
}
//ボロノイ図2D
float voronoi2(in float2 vec)
{
	float2 ivec = floor(vec);//floorは少数の整数のみを取り出す
	float2 fvec = frac(vec);//fracは少数の少数のみを取り出す

	float value = 1.0;
	//周りの9ブロックを調べる
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			float2 offset = float2(x, y);

			float2 position;
			//点の座標
			position = random22(ivec + offset);
			//その点の座標までの距離を求める
			float dist = distance(position + offset, fvec);
			//一番近い点の位置を得る
			value = min(value, dist);
		}
	}

	return value;
}
//ボロノイ図3D
float voronoi3(in float3 vec)
{
	float3 ivec = floor(vec);//floorは少数の整数のみを取り出す
	float3 fvec = frac(vec);//fracは少数の少数のみを取り出す

	float value = 1.0;
	//周りの9ブロックを調べる
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			for (int z = -1; z <= 1; z++)
			{
				float3 offset = float3(x, y, z);

				float3 position;
				//点の座標
				position = random33(ivec + offset);
				//その点の座標までの距離を求める
				float dist = distance(position + offset, fvec);
				//一番近い点の位置を得る
				value = min(value, dist);
			}
		}
	}

	return value;
}
//バリューノイズ2D
float valueNoise2(in float2 vec)
{
	float2 ivec = floor(vec);
	float2 fvec = frac(vec);
	//格子状の位置の乱数を得る 左上から右下
	float a = random2(ivec + float2(0.0, 0.0));
	float b = random2(ivec + float2(1.0, 0.0));
	float c = random2(ivec + float2(0.0, 1.0));
	float d = random2(ivec + float2(1.0, 1.0));
	//smoothstepは滑らかに補間する関数(エルミート補間)
	fvec = smoothstep(0.0, 1.0, fvec);
	//lerpは線形補間
	return lerp(lerp(a, b, fvec.x), lerp(c, d, fvec.x), fvec.y);
}
//バリューノイズ3D
float valueNoise3(in float3 vec)
{
	float3 ivec = floor(vec);
	float3 fvec = frac(vec);
	//格子状の位置の乱数を得る 左上から右下
	float a = random3(ivec + float3(0.0, 0.0, 0.0));
	float b = random3(ivec + float3(1.0, 0.0, 0.0));
	float c = random3(ivec + float3(0.0, 1.0, 0.0));
	float d = random3(ivec + float3(1.0, 1.0, 0.0));

	float e = random3(ivec + float3(0.0, 0.0, 1.0));
	float f = random3(ivec + float3(1.0, 0.0, 1.0));
	float g = random3(ivec + float3(0.0, 1.0, 1.0));
	float h = random3(ivec + float3(1.0, 1.0, 1.0));
	//smoothstepは滑らかに補間する関数(エルミート補間)
	fvec = smoothstep(0.0, 1.0, fvec);
	//lerpは線形補間
	float v1 = lerp(lerp(a, b, fvec.x), lerp(c, d, fvec.x), fvec.y);
	float v2 = lerp(lerp(e, f, fvec.x), lerp(g, h, fvec.x), fvec.y);
	float v3 = lerp(v1, v2, fvec.z);

	return v3;
}
//パーリンノイズ2D
float perlinNoise2(in float2 vec)
{
	float2 ivec = floor(vec);
	float2 fvec = frac(vec);
	//格子状の位置の乱数を得る 左上から右下
	float a = dot(random22(ivec + float2(0.0, 0.0))*2.0 - 1.0, fvec - float2(0.0, 0.0));
	float b = dot(random22(ivec + float2(1.0, 0.0))*2.0 - 1.0, fvec - float2(1.0, 0.0));
	float c = dot(random22(ivec + float2(0.0, 1.0))*2.0 - 1.0, fvec - float2(0.0, 1.0));
	float d = dot(random22(ivec + float2(1.0, 1.0))*2.0 - 1.0, fvec - float2(1.0, 1.0));
	//smoothstepは滑らかに補間する関数(エルミート補間)
	fvec = smoothstep(0.0, 1.0, fvec);
	//lerpは線形補間
	return lerp(lerp(a, b, fvec.x), lerp(c, d, fvec.x), fvec.y);
}
//パーリンノイズ3D
float perlinNoise3(in float3 vec)
{
	float3 ivec = floor(vec);
	float3 fvec = frac(vec);
	//格子状の位置の乱数を得る 左上から右下
	float a = dot(random33(ivec + float3(0.0, 0.0, 0.0))*2.0 - 1.0, fvec - float3(0.0, 0.0, 0.0));
	float b = dot(random33(ivec + float3(1.0, 0.0, 0.0))*2.0 - 1.0, fvec - float3(1.0, 0.0, 0.0));
	float c = dot(random33(ivec + float3(0.0, 1.0, 0.0))*2.0 - 1.0, fvec - float3(0.0, 1.0, 0.0));
	float d = dot(random33(ivec + float3(1.0, 1.0, 0.0))*2.0 - 1.0, fvec - float3(1.0, 1.0, 0.0));
	float e = dot(random33(ivec + float3(0.0, 0.0, 1.0))*2.0 - 1.0, fvec - float3(0.0, 0.0, 1.0));
	float f = dot(random33(ivec + float3(1.0, 0.0, 1.0))*2.0 - 1.0, fvec - float3(1.0, 0.0, 1.0));
	float g = dot(random33(ivec + float3(0.0, 1.0, 1.0))*2.0 - 1.0, fvec - float3(0.0, 1.0, 1.0));
	float h = dot(random33(ivec + float3(1.0, 1.0, 1.0))*2.0 - 1.0, fvec - float3(1.0, 1.0, 1.0));
	//smoothstepは滑らかに補間する関数(エルミート補間)
	fvec = smoothstep(0.0, 1.0, fvec);

	float v1 = lerp(lerp(a, b, fvec.x), lerp(c, d, fvec.x), fvec.y);
	float v2 = lerp(lerp(e, f, fvec.x), lerp(g, h, fvec.x), fvec.y);
	float v3 = lerp(v1, v2, fvec.z);
	//lerpは線形補間
	return v3;
}

//非整数ブラウン運動2D
float fbm2(in float2 vec, int octave,float offset = 0.0)
{
	float value = 0.0f;
	float amplitude = 1.0;
	float frequency = 0.0;
	//octave
	for (int i = 0; i < octave; i++)
	{
		//細かい山を小さくして足す
		value += amplitude * perlinNoise2(vec + offset);
		vec *= 2.0;
		amplitude *= 0.5;
	}

	return value;
}

float turbulence2(in float2 vec, int octave, float offset = 0.0)
{
	float value = 0.0f;
	float amplitude = 0.5;
	float frequency = 0.0;
	//octave
	for (int i = 0; i < octave; i++)
	{
		//細かい山を小さくして足す
		value += amplitude * abs(perlinNoise2(vec + offset));
		vec *= 2.0;
		amplitude *= 0.5;
	}

	return value;
}

//非整数ブラウン運動3D
float fbm3(in float3 vec, int octave)
{
	float value = 0.0f;
	float amplitude = 1.0;
	float frequency = 0.0;
	//octave
	for (int i = 0; i < octave; i++)
	{
		//細かい山を小さくして足す
		value += amplitude * perlinNoise3(vec);
		vec *= 2.0;
		amplitude *= 0.5;
	}

	return value;
}

//2Dの空
float noise(in float2 p) {
	const float K1 = 0.366025404; // (sqrt(3)-1)/2;
	const float K2 = 0.211324865; // (3-sqrt(3))/6;
	float2 i = floor(p + (p.x + p.y)*K1);
	float2 a = p - i + (i.x + i.y)*K2;
	float2 o = (a.x > a.y) ? float2(1.0, 0.0) : float2(0.0, 1.0); //float2 of = 0.5 + 0.5*float2(sign(a.x-a.y), sign(a.y-a.x));
	float2 b = a - o + K2;
	float2 c = a - 1.0 + 2.0*K2;
	float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
	float3 n = h * h*h*h*float3(dot(a, random22(i + 0.0)), dot(b, random22(i + o)), dot(c, random22(i + 1.0)));
	return dot(n, (70.0));
}

float fbm(float2 n) {
	const float4 m = float4(1.6, 1.2, -1.2, 1.6);
	float total = 0.0, amplitude = 0.1;
	for (int i = 0; i < 7; i++) {
		total += noise(n) * amplitude;
		n = m * n;
		amplitude *= 0.4;
	}
	return total;
}

float noise3(in float3 p) {
	const float K1 = 0.366025404; // (sqrt(3)-1)/2;
	const float K2 = 0.211324865; // (3-sqrt(3))/6;
	float3 i = floor(p + (p.x + p.y)*K1);
	float3 a = p - i + (i.x + i.y)*K2;
	float2 o = (a.x > a.y) ? float2(1.0, 0.0) : float2(0.0, 1.0); //float2 of = 0.5 + 0.5*float2(sign(a.x-a.y), sign(a.y-a.x));
	float2 b = a - o + K2;
	float2 c = a - 1.0 + 2.0*K2;
	float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
	float3 n = h * h*h*h*float3(dot(a, random22(i + 0.0)), dot(b, random22(i + o)), dot(c, random22(i + 1.0)));
	return dot(n, (70.0));
}

float fbm333(in float3 vec, int octave)
{
	const float4 m = float4(1.6, 1.2, -1.2, 1.6);
	float total = 0.0, amplitude = 0.1;
	for (int i = 0; i < octave; i++) {
		total += noise3(vec) * amplitude;
		vec = m * vec;
		amplitude *= 0.4;
	}
	return total;
}

//////////////////

float3 voro(float2 uv)
{
	float2 uv_id = floor(uv);
	float2 uv_st = frac(uv);

	float2 m_diff;
	float2 m_point;
	float2 m_neighbor;
	float m_dist = 10.;
	float PI = 3.14159;

	for (int j = -1; j <= 1; j++)
	{
		for (int i = -1; i <= 1; i++)
		{
			float2 neighbor = float2(float(i), float(j));
			float2 p = random22(uv_id + neighbor);
			p = 0.5 + 0.5*sin(2.*PI*p);
			float2 diff = neighbor + p - uv_st;

			float dist = length(diff);
			if (dist < m_dist)
			{
				m_dist = dist;
				m_point = p;
				m_diff = diff;
				m_neighbor = neighbor;
			}
		}
	}

	m_dist = 10.;
	for (int j = -2; j <= 2; j++)
	{
		for (int i = -2; i <= 2; i++)
		{
			if (i == 0 && j == 0) continue;
			float2 neighbor = m_neighbor + float2(float(i), float(j));
			float2 p = random22(uv_id + neighbor);
			p = 0.5 + 0.5*sin(p*2.*PI);
			float2 diff = neighbor + p - uv_st;
			float dist = dot(0.5*(m_diff + diff), normalize(diff - m_diff));
			m_point = p;
			m_dist = min(m_dist, dist);
		}
	}

	return float3(m_point, m_dist);
}

float3 voro3(float3 pos)
{
	float3 uv_id = floor(pos);
	float3 uv_st = frac(pos);

	float3 m_diff;
	float3 m_point;
	float3 m_neighbor;
	float m_dist = 10.;
	float PI = 3.14159;
	for (int j = -1; j <= 1; j++)
	{
		for (int i = -1; i <= 1; i++)
		{
			for (int k = -1; k <= 1; k++)
			{
				float3 neighbor = float3(float(i), float(j), float(k));
				float3 p = random3(uv_id + neighbor);
				p = 0.5 + 0.5*sin(2.*PI*p);
				float3 diff = neighbor + p - uv_st;

				float dist = length(diff);
				if (dist < m_dist)
				{
					m_dist = dist;
					m_point = p;
					m_diff = diff;
					m_neighbor = neighbor;
				}
			}
		}
	}

	m_dist = 10.;
	for (int j = -2; j <= 2; j++)
	{
		for (int i = -2; i <= 2; i++)
		{
			for (int k = -2; k <= 2; k++)
			{
				if (i == 0 && j == 0 && k == 0) continue;
				float3 neighbor = m_neighbor + float3(float(i), float(j), float(k));
				float3 p = random3(uv_id + neighbor);
				p = 0.5 + 0.5*sin(p*2.*PI);
				float3 diff = neighbor + p - uv_st;
				float dist = dot(0.5*(m_diff + diff), normalize(diff - m_diff));
				m_point = p;
				m_dist = min(m_dist, dist);
			}
		}
	}

	return float3(m_point + m_dist);
}

float bias(float b, float x)
{
	return pow(x, log(b) / log(0.5));
}

float gain(float g, float x)
{
	if (x < 0.5)
	{
		return bias(1.0 - g, 2.0 * x) / 2.0;
	}
	else
	{
		return 1.0 - bias(1.0 - g, 2.0 - 2.0 * x) / 2.0;
	}
}

float gammacorrect(float gamma, float x)
{
	return pow(x, 1.0 / gamma);
}