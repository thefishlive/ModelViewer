cbuffer WVPBuffer
{
	float4x4 WVP;
	float4x4 World;
};

struct Light
{
	float3 dir;
	float4 ambient;
	float4 diffuse;
};

cbuffer LightBuffer
{
	Light light;
};

Texture2D Texture;
SamplerState Sampler;

struct VS_OUTPUT
{
	float4 Pos: SV_POSITION;
	float2 TexCoord: TEXCOORD;
	float3 Normal : NORMAL;
};

VS_OUTPUT VS(float4 position : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL)
{
	VS_OUTPUT output;

	output.Pos = mul(position, WVP);
	output.TexCoord = texCoord;
	output.Normal = mul(normal, World);

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.Normal = normalize(input.Normal);

	float4 diffuse = Texture.Sample(Sampler, input.TexCoord);
	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.Normal) * light.diffuse * diffuse);

	return float4(finalColor, diffuse.a);
}