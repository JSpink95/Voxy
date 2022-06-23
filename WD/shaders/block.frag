#version 450 core

struct VertexOutput
{
	vec3 worldPosition, normal;
	vec2 texcoord;
	float textureId;
};

layout(location = 0) in VertexOutput f_In;
layout(location = 0) out vec4 o_Color;
layout(location = 1) out vec3 o_Position;
layout(location = 2) out vec3 o_Normal;

layout(binding = 0) uniform sampler2D _Textures[3];

void main()
{
	vec4 color = vec4(0.0f);
	int textureId = int(round(f_In.textureId));

	switch(textureId)
	{
		case 0: color = texture(_Textures[0], f_In.texcoord); break;
		case 1: color = texture(_Textures[1], f_In.texcoord); break;
		case 2: color = texture(_Textures[2], f_In.texcoord); break;
	}

	o_Color = color;
	o_Position = f_In.worldPosition;
	o_Normal = f_In.normal;
}