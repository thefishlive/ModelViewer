#define NUM_LIGHTS 3

#define LIGHT_ 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_POINT 2
#define LIGHT_SPOT 3

struct Light
{
	int type;
	float3 position;

	int enabled;
	float3 dir;

	float range;
	float3 att;

	float4 ambient;
	float4 diffuse;
};

cbuffer LightBuffer : register(cb0)
{
	Light light[NUM_LIGHTS];
};

cbuffer WVPBuffer : register(cb1)
{
	float4x4 WVP;
	float4x4 World;
	
	bool hasTexture;
	bool hasNormalMap;
};


Texture2D Texture;
Texture2D NormalMap;
SamplerState Sampler;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
	float3 Tangent : TANGENT;
};

float3 DirectionalLight(VS_OUTPUT input, Light light)
{
	float4 finalColor;

	finalColor = light.ambient;
	finalColor += saturate(dot(light.dir, input.Normal) * light.diffuse);

	return finalColor;
}

float3 PointLight(VS_OUTPUT input, Light light)
{
	float3 finalColor = { 0.0f, 0.0f, 0.0f };

	float3 distanceVector = light.position - input.WorldPos;
	float dist = length(distanceVector);

	float3 finalAmbient = light.ambient;

	if (dist > light.range)
	{
		return float3(1.0f, 1.0f, 1.0f);
	}

	distanceVector /= dist;
	float howMuchLight = dot(distanceVector, input.Normal);

	if (howMuchLight >= 0.0f)
	{
		finalColor += howMuchLight * light.diffuse;
		finalColor /= light.att[0] + (light.att[1] * dist) + (light.att[2] * (dist * dist));
	}

	return saturate(finalColor + finalAmbient);
}

float4 main(VS_OUTPUT input) : SV_TARGET
{
	if (hasNormalMap == true)
	{
		float4 normalMap = NormalMap.Sample(Sampler, input.TexCoord);
		normalMap = (2.0f * normalMap) - 1.0f;
		input.Tangent = normalize(input.Tangent - dot(input.Tangent, input.Normal) * input.Normal);
		float3 biTangent = cross(input.Normal, input.Tangent);
		float3x3 texSpace = float3x3(input.Tangent, biTangent, input.Tangent);
		input.Normal = normalize(mul(normalMap.xyz, texSpace));
	}

	float4 diffuse = Texture.Sample(Sampler, input.TexCoord);
	float3 finalColor = { 0.0f, 0.0f, 0.0f };
	float3 colors[NUM_LIGHTS];
	
	[unroll]
	for (uint i = 0; i < NUM_LIGHTS; i++)
	{
		if (light[i].enabled != 1)
		{

		}
		else if (light[i].type == LIGHT_DIRECTIONAL)
		{
			colors[i] = DirectionalLight(input, light[i]);
		}
		else if (light[i].type == LIGHT_POINT)
		{
			colors[i] = PointLight(input, light[i]);
		}
		else if (light[i].type == LIGHT_SPOT)
		{

		}
		
		finalColor += saturate(colors[i] * diffuse.rgb);
	}

	finalColor = saturate(finalColor);
	return float4(finalColor, diffuse.a);
}