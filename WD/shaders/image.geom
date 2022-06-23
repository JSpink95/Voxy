#version 450 core

layout(location = 0) in VertexData
{
	vec2 position;
} g_In[];

layout(location = 0) out GeometryData
{
	vec2 texcoord;
} g_Out;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

uniform float _Size;
uniform float _Aspect;

uniform mat4 _Projection;
uniform mat4 _Transform;

void main()
{
	float width = _Size;
	float height = width * _Aspect;

	vec2 position = g_In[0].position;

	g_Out.texcoord = vec2(0.0f, 0.0f);
	gl_Position = _Projection * _Transform * vec4(position.x, position.y, 0.0f, 1.0f);
	EmitVertex();

	g_Out.texcoord = vec2(1.0f, 0.0f);
	gl_Position = _Projection * _Transform * vec4(position.x + width, position.y, 0.0f, 1.0f);
	EmitVertex();

	g_Out.texcoord = vec2(0.0f, 1.0f);
	gl_Position = _Projection * _Transform * vec4(position.x, position.y + height, 0.0f, 1.0f);
	EmitVertex();
	
	g_Out.texcoord = vec2(1.0f, 1.0f);
	gl_Position = _Projection * _Transform * vec4(position.x + width, position.y + height, 0.0f, 1.0f);
	EmitVertex();
}
