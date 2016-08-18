
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
};

Texture2D Texture;
SamplerState Sampler;

float4 main(VS_OUTPUT input) : SV_TARGET
{
	return Texture.Sample(Sampler, input.TexCoord);
}