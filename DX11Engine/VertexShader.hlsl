
cbuffer WVPBuffer : register(cb0)
{
	float4x4 WVP;
	float4x4 World;
	
	bool hasTexture;
	bool hasNormalMap;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
	float3 tangent : TANGENT;
};


VS_OUTPUT main(float4 position : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT)
{
	VS_OUTPUT output;

	output.WorldPos = mul(position, World).xyz;
	output.Pos = mul(position, WVP);
	output.TexCoord = texCoord;
	output.Normal = normalize(mul(float4(normal, 0.0f), World)).xyz;
	output.tangent = normalize(mul(float4(tangent, 0.0f), World)).xyz;

	return output;
}
