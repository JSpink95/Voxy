#version 450 core

struct VertexOutput
{
	vec4 color;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(location = 0) out VertexOutput v_Out;

uniform mat4 _Projection;
uniform mat4 _View;
uniform mat4 _Transform;

void main()
{
	gl_Position = _Projection * _View * _Transform * vec4(a_Position, 1.0f);
	v_Out.color = a_Color;
}
