#version 450 core

layout(location = 0) in vec2 a_Position;

layout(location = 0) out VertexData
{
	vec2 texcoord;
} v_Out;

void main()
{
	gl_Position = vec4(a_Position, 0.0f, 1.0f);
	v_Out.texcoord = a_Position * 0.5f + 0.5f;
}
