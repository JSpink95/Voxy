#version 450 core

struct VertexOutput
{
	vec2 texcoord;
};

layout(location = 0) in VertexOutput f_In;
layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D _Texture;

void main()
{
	vec4 color = texture(_Texture, f_In.texcoord);
	o_Color = color;
}