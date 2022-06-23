#version 450 core

struct VertexOutput
{
    vec3 position, textures;
    int neighbours;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in ivec3 a_TextureIds;
layout(location = 2) in int a_NeighbourActive;

layout(location = 0) out VertexOutput v_Out;

void main()
{
    v_Out.position = a_Position;
    v_Out.textures = vec3(a_TextureIds);
    v_Out.neighbours = a_NeighbourActive;
}