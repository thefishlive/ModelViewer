#define NUM_LIGHTS 3

#define LIGHT_ 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_POINT 2
#define LIGHT_SPOT 3

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
	Light light[NUM_LIGHTS];
};

Texture2D Texture;
SamplerState Sampler;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
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
	float4 diffuse = Texture.Sample(Sampler, input.TexCoord);
	float3 finalColor = { 0.0f, 0.0f, 0.0f };
	float3 colors[NUM_LIGHTS];
	
	[unroll]
	for (uint i = 0; i < NUM_LIGHTS; i++)
	{
		if (light[i].type == LIGHT_DIRECTIONAL)
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
		
		finalColor += colors[i];
	}

	finalColor = saturate(finalColor) * diffuse.rgb;
	return float4(finalColor, diffuse.a);
}