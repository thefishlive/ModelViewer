#define NUM_LIGHTS 1

#define LIGHT_UNLIT 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_POINT 2
#define LIGHT_SPOT 3

cbuffer WVPBuffer
{
	float4x4 WVP;
	float4x4 World;
};

struct Light
{
	int type;

	float3 position;
	float3 dir;
	float pad;

	float range;
	float3 att;

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
	float3 WorldPos : POSITION;
};

float3 DirectionalLight(VS_OUTPUT input, float4 diffuse, Light light)
{
	float4 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.Normal) * light.diffuse * diffuse);

	return finalColor;
}

float3 PointLight(VS_OUTPUT input, float4 diffuse, Light light)
{
	float3 finalColor = { 0.0f, 0.0f, 0.0f };

	float3 distanceVector = light.position - input.WorldPos;
	float dist = length(distanceVector);

	float3 finalAmbient = diffuse * light.ambient;

	if (dist > light.range)
	{
		return float3(1.0f, 1.0f, 1.0f);
	}

	distanceVector /= dist;
	float howMuchLight = dot(distanceVector, input.Normal);

	if (howMuchLight >= 0.0f)
	{
		finalColor += howMuchLight * diffuse * light.diffuse;
		finalColor /= light.att[0] + (light.att[1] * dist) + (light.att[2] * (dist * dist));
	}

	return saturate(finalColor + finalAmbient);
}

VS_OUTPUT VS(float4 position : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL)
{
	VS_OUTPUT output;

	output.WorldPos = mul(position, World);
	output.Pos = mul(position, WVP);
	output.TexCoord = texCoord;
	output.Normal = mul(normal, World);

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.Normal = normalize(input.Normal);

	float4 diffuse = Texture.Sample(Sampler, input.TexCoord);
	float3 finalColor = { 0.0f, 0.0f, 0.0f };
	
	if (light.type == LIGHT_UNLIT)
	{
		finalColor = diffuse;
	}
	else if (light.type == LIGHT_DIRECTIONAL)
	{
		finalColor = DirectionalLight(input, diffuse, light);
	}
	else if (light.type == LIGHT_POINT)
	{
		finalColor = PointLight(input, diffuse, light);
	}
	else if (light.type == LIGHT_SPOT)
	{

	}

	return float4(finalColor, diffuse.a);
}