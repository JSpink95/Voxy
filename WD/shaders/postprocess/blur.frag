#version 450 core

struct VertexOutput
{
	vec2 texcoord;
};

layout(location = 0) in VertexOutput f_In;
layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D _Texture;

void main()
{
	vec2 texelSize = 1.0f / vec2(textureSize(_Texture, 0));
    float result = 0.0f;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(_Texture, f_In.texcoord + offset).r;
        }
    }

    o_Color = vec4(result / (4.0 * 4.0));
}
