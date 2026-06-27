
cbuffer global_Line:register(b0)
{
	matrix g_mRayWVP;
	float4 g_Color;
};

float4 main(float4 Pos : SV_POSITION) : SV_Target
{
	return g_Color;
}