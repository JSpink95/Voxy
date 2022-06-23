#version 450 core

#define KERNEL_SIZE 64

struct VertexOutput
{
	vec2 texcoord;
};

layout(location = 0) in VertexOutput f_In;
layout(location = 0) out vec4 o_AO;

layout(binding = 0) uniform sampler2D _Position;
layout(binding = 1) uniform sampler2D _Normal;
layout(binding = 2) uniform sampler2D _Noise;

uniform vec3 _Samples[KERNEL_SIZE];
uniform mat4 _Projection;
uniform mat4 _View;

const float _Radius = 0.1f;
const float _Bias = 0.025f;
const vec2 _NoiseScale = vec2(1920.0 / 4.0, 1080.0 / 4.0);

void main()
{
	vec3 position = (_View * vec4(texture(_Position, f_In.texcoord).xyz, 1.0f)).xyz;
	vec3 normal = mat3(_View) * texture(_Normal, f_In.texcoord).xyz;
	vec3 random = texture(_Noise, f_In.texcoord).xyz;

	vec3 tangent   = normalize(random - normal * dot(random, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN       = mat3(tangent, bitangent, normal);  

	float occlusion = 0.0f;
	for(int i = 0; i < KERNEL_SIZE; ++i)
	{
	    // get sample position
	    vec3 samplePos = TBN * _Samples[i]; // from tangent to view-space
	    samplePos = position + samplePos * _Radius;

		vec4 offset = vec4(samplePos, 1.0f);
		offset      = _Projection * offset;		// from view to clip-space
		offset.xyz /= offset.w;					// perspective divide
		offset.xyz  = offset.xyz * 0.5f + 0.5f; // transform to range 0.0 - 1.0 

		vec3 offsetPosition = texture(_Position, offset.xy).xyz;
		offsetPosition = (_View * vec4(offsetPosition, 1.0f)).xyz;

		float sampleDepth = offsetPosition.z; 
		occlusion += (sampleDepth >= samplePos.z + _Bias ? 1.0 : 0.0); 

		float rangeCheck = smoothstep(0.0, 1.0, _Radius / abs(position.z - sampleDepth));
		occlusion       += (sampleDepth >= samplePos.z + _Bias ? 1.0 : 0.0) * rangeCheck;    
	}

	occlusion = 1.0f - (occlusion / float(KERNEL_SIZE));
	o_AO = vec4(occlusion);
}
