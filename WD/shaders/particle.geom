#version 450 core

struct VertexOutput
{
	vec4 color;
};

struct GeometryOutput
{
	vec4 color;
};

layout(location = 0) in VertexOutput g_In[];
layout(location = 0) out GeometryOutput g_Out;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;


void main()
{
	const float Aspect = 1920.0f / 1080.0f;
	const float SizeX = 0.01f;
	const float SizeY = SizeX * Aspect;

	g_Out.color = g_In[0].color;
	vec4 screenPosition = gl_in[0].gl_Position;

	gl_Position = screenPosition + vec4(-SizeX, -SizeY, 0.0f, 0.0f);
	EmitVertex();

	gl_Position = screenPosition + vec4( SizeX, -SizeY, 0.0f, 0.0f);
	EmitVertex();

	gl_Position = screenPosition + vec4(-SizeX,  SizeY, 0.0f, 0.0f);
	EmitVertex();

	gl_Position = screenPosition + vec4( SizeX,  SizeY, 0.0f, 0.0f);
	EmitVertex();

	EndPrimitive();
}
