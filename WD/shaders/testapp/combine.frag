#version 450 core

layout(location = 0) in VertexData
{
	vec2 texcoord;
} f_In;

layout (location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D _Albedo;
layout(binding = 1) uniform sampler2D _Position;
layout(binding = 2) uniform sampler2D _Normal;
layout(binding = 3) uniform sampler2D _Shadow;

uniform mat4 _ShadowMatrix;

uniform vec3 _LightPosition = vec3(-2.0f, 4.0f, -1.0f);

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
            float pcfDepth = texture(_Shadow, projCoords.xy + vec2(x, y) * texelSize).r; 
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
	vec2 texcoord = f_In.texcoord;

	vec4 albedo = texture(_Albedo, texcoord);
	vec3 position = texture(_Position, texcoord).xyz;
    vec3 normal = texture(_Normal, texcoord).xyz;

    vec3 lightDirection = normalize(_LightPosition);
    float nl = max(dot(normal, lightDirection), 0.0f);

	vec4 lightSpacePosition = _ShadowMatrix * vec4(position, 1.0f);
    float shadow = 1.0f - ShadowCalculationPCF(lightSpacePosition);

    o_Color = albedo * max(shadow * nl, 0.2f);
}
