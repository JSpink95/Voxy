#version 450 core

struct VertexOutput
{
	vec3 position, normal;
	vec2 texcoord;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;

layout(location = 0) out VertexOutput v_Out;

uniform mat4 _Projection;
uniform mat4 _View;
uniform mat4 _Transform;

void main()
{
	v_Out.position = (_Transform * vec4(a_Position, 1.0f)).xyz;
	v_Out.normal = mat3(_Transform) * a_Normal;
	v_Out.texcoord = a_Texcoord;
	gl_Position = _Projection * _View * vec4(v_Out.position, 1.0f);
}