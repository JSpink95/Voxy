#version 450 core

layout(location = 0) in GeometryData
{
	vec2 texcoord;
} f_In;

layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D _Texture;

float LinearizeDepth(float d,float zNear,float zFar)
{
    return zNear * zFar / (zFar + d * (zNear - zFar));
}

void main()
{
	vec2 texcoord = f_In.texcoord;

	vec4 color = texture(_Texture, texcoord);
	o_Color = color;
}