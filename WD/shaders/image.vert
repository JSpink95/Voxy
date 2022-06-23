#version 450 core

layout(location = 0) in vec2 a_Position;

layout(location = 0) out VertexData
{
	vec2 position;
} v_Out;

void main()
{
	v_Out.position = a_Position;
}