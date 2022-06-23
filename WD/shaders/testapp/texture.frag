#version 450 core

layout(location = 0) in VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} f_In;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec3 o_Position;
layout(location = 2) out vec3 o_Normal;

layout(binding = 0) uniform sampler2D _Texture;

void main()
{
	o_Albedo = texture(_Texture, f_In.texcoord);
	o_Position = f_In.position;
	o_Normal = f_In.normal;
}
