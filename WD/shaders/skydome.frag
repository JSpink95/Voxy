#version 450 core

struct VertexOutput
{
	float height;
};

layout(location = 0) in VertexOutput f_In;
layout(location = 0) out vec4 o_Color;

vec4 _HorizonColor = vec4(1.0f, 0.8f, 0.05f, 1.0f);
vec4 _SkyColor = vec4(0.2f, 0.015f, 1.0f, 1.0f);

void main()
{
	float amount = smoothstep(-1.0f, 1.0f, f_In.height) * 0.75f;
	vec4 blended = mix(_HorizonColor, _SkyColor, amount);
	o_Color = blended;
}