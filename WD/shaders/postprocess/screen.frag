#version 450 core

#define SSAO_ENABLED

#define MAX_LIGHTS 16

struct VertexOutput
{
	vec2 texcoord;
};

struct Light
{
	vec4 data;
	vec4 color;
	int type;
};

layout(location = 0) in VertexOutput f_In;
layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D _AlbedoTexture;
layout(binding = 1) uniform sampler2D _PositionTexture;
layout(binding = 2) uniform sampler2D _NormalTexture;
layout(binding = 3) uniform sampler2D _DepthTexture;
layout(binding = 4) uniform sampler2D _SSAOTexture;
layout(binding = 5) uniform sampler2D _Shadow;

uniform mat4 _ShadowProjectionMatrix;
uniform mat4 _ShadowViewMatrix;

uniform float _LightCount;
layout(std140, binding = 5) uniform Lights
{
	Light _Lights[32];
};

float LinearizeDepth(float d,float zNear,float zFar)
{
    return zNear * zFar / (zFar + d * (zNear - zFar));
}

float DirectionalLight(int light, vec3 position, vec3 normal)
{
	vec3 lightDirection = normalize(_Lights[light].data.xyz);
	float intensity = _Lights[light].data.w;

	return max(0.0f, dot(lightDirection, normal)) * intensity;
}

float PointLight(int light, vec3 position, vec3 normal)
{
	vec3 lightPosition = _Lights[light].data.xyz;
	float lightRadius = _Lights[light].data.w;
	
	float dist = distance(position, lightPosition);
	if (dist < lightRadius)
	{
		float attenuation = clamp(1.0f - dist * dist / (lightRadius * lightRadius), 0.0f, 1.0f);
		vec3 surfaceToLight = normalize(lightPosition - position);
		return attenuation * max(0.0f, dot(normal, surfaceToLight));
	}

	return 0.0f;
}

float GetBrightness(int lindex, vec3 position, vec3 normal)
{
	float brightness = 0.0f;

	vec4 lightColor = _Lights[lindex].color;
	int lightType = _Lights[lindex].type;
	
	switch(lightType)
	{
		// directional
		case 0:
		{
			brightness = DirectionalLight(lindex, position, normal);
		} break;
	
		// point
		case 1:
		{
			brightness = 0.0f;//PointLight(lindex, position, normal);
		} break;
	}

	return brightness;
}

float SampleShadowTexture(vec2 texcoord)
{
	if (texcoord.x < 0 || texcoord.x > 1 || texcoord.y < 0 || texcoord.y > 1)
		return 1.0f;

	return texture(_Shadow, texcoord).r;
}

float ShadowCalculationPCF(vec4 lightSpacePosition)
{
    // perform perspective divide
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(_Shadow, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // calculate a bias to account for slight innacuracies in depth sampling
    float bias = 0.005f;//max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f); 

    // check whether current frag pos is in shadow
    //float shadow = (currentDepth - bias) > closestDepth  ? 0.5f : 0.0f;

    float shadow = 0.0;
    vec2 texelSize = 1.0f / textureSize(_Shadow, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = SampleShadowTexture(projCoords.xy + vec2(x, y) * texelSize); 
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0f;

    return shadow;
}

float ShadowCalculation(vec4 lightSpacePosition)
{
    // perform perspective divide
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(_Shadow, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // calculate a bias to account for slight innacuracies in depth sampling
    float bias = 0.005f;//max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f); 

    // check whether current frag pos is in shadow
    float shadow = (currentDepth - bias) > closestDepth  ? 0.5f : 0.0f;

    return shadow;
}

void main()
{
	vec4 albedo = texture(_AlbedoTexture, f_In.texcoord).rgba;
	vec3 position = texture(_PositionTexture, f_In.texcoord).xyz;
	vec3 normal = normalize(texture(_NormalTexture, f_In.texcoord).xyz);
	//float depth = texture(_DepthTexture, f_In.texcoord).r;
	//depth = LinearizeDepth(depth, 0.01f, 1000.0f);

#ifdef SSAO_ENABLED
	float ssao = texture(_SSAOTexture, f_In.texcoord).r;
#endif

	o_Color = albedo;
	
	float brightness = 0.0f;
	float shadow = 1.0f;

#if 1
	mat4 shadowMatrix = _ShadowProjectionMatrix * _ShadowViewMatrix;
	vec4 lightSpacePosition = shadowMatrix * vec4(position, 1.0f);
    shadow = 1.0f - ShadowCalculationPCF(lightSpacePosition);
#endif

#if 1
	vec4 finalLightColor = vec4(0.0f);

	int lightCount = int(_LightCount);
	for (int lindex = 0; lindex < lightCount; ++lindex)
	{
		brightness += GetBrightness(lindex, position, normal);
	}
#endif

	o_Color = albedo * max(brightness * shadow, 0.1f);

#ifdef SSAO_ENABLED
	o_Color *= ssao;
#endif
	
	//o_Color = vec4(ssao);
	//o_Color = vec4(position, 1.0f);
	//o_Color = vec4(normal * 0.5f + 0.5f, 1.0f);
	//o_Color = vec4(depth);
}