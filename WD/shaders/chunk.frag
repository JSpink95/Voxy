#version 450 core

struct GeometryOutput
{
    vec3 position, normal;
    vec2 texcoord;
    float textureId;
};

layout(location = 0) in GeometryOutput f_In;
layout(location = 0) out vec4 o_Color;
layout(location = 1) out vec3 o_Position;
layout(location = 2) out vec3 o_Normal;

layout(binding = 0) uniform sampler2D _Texture[5];

void main()
{
    int textureId = int(round(f_In.textureId));
    vec4 textureColor = vec4(0.0f);

    switch(textureId)
    {
        default:
        case 0: textureColor = texture(_Texture[0], f_In.texcoord); break;
        case 1: textureColor = texture(_Texture[1], f_In.texcoord); break;
        case 2: textureColor = texture(_Texture[2], f_In.texcoord); break;
        case 3: textureColor = texture(_Texture[3], f_In.texcoord); break;
        case 4: textureColor = texture(_Texture[4], f_In.texcoord); break;
    }

    o_Color = vec4(textureColor.rgb, 1.0f);
    o_Position = f_In.position;
    o_Normal = f_In.normal;
}
