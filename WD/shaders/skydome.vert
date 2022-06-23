#version 450 core

struct VertexOutput
{
	float height;
};

layout(location = 0) in vec3 a_Position;
layout(location = 0) out VertexOutput v_Out;

uniform mat4 _Projection;
uniform mat4 _View;

void main()
{
	mat4 view = _View;
	view[3] = vec4(0, 0, 0, 1);

	gl_Position = _Projection * view * vec4(a_Position, 1.0f);
	v_Out.height = a_Position.y;
}
