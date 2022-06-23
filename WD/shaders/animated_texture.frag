#version 450 core

struct VertexOutput
{
	vec3 position, normal;
	vec2 texcoord;
};

layout(location = 0) in VertexOutput f_In;
layout(location = 0) out vec4 o_Color;
layout(location = 1) out vec3 o_Position;
layout(location = 2) out vec3 o_Normal;

layout(binding = 0) uniform sampler2D _Texture;
uniform float _SubTextureIndex;

void main()
{
	ivec2 size = textureSize(_Texture, 0);

	float usize = 1.0f / float(size.x) * 16.0f;
	float vsize = 1.0f;

	float u = (f_In.texcoord.x + _SubTextureIndex) * usize;
	float v = f_In.texcoord.y;

	o_Color = texture(_Texture, vec2(u, v));
	o_Position = f_In.position;
	o_Normal = f_In.normal;
}