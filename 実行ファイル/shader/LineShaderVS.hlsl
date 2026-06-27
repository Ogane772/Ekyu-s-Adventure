
cbuffer global_Line:register(b0)
{
	matrix g_mRayWVP;
	float4 g_Color;
};

float4 main(float4 Pos : POSITION) : SV_POSITION
{
	Pos = mul(Pos,g_mRayWVP);
	return Pos;
}