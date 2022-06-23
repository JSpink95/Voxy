#version 450 core

struct GeometryOutput
{
	vec4 color;
};

layout(location = 0) in GeometryOutput f_In;
layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = f_In.color;
}
