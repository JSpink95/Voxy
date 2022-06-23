#version 450 core

struct VertexOutput
{
	vec2 texcoord;
};

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_Texcoord;

layout(location = 0) out VertexOutput v_Out;

uniform mat4 _Projection;
uniform mat4 _Transform;

void main()
{
	gl_Position = _Projection * _Transform * vec4(a_Position, 0.0f, 1.0f);
	v_Out.texcoord = a_Texcoord;
}
