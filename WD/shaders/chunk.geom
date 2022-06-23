#version 450 core

struct VertexOutput
{
    vec3 position, textures;
    int neighbours;
};

struct GeometryOutput
{
    vec3 position, normal;
    vec2 texcoord;
    float textureId;
};

layout(points) in;
layout(max_vertices=12, triangle_strip) out;

layout(location = 0) in VertexOutput v_Out[];
layout(location = 0) out GeometryOutput g_Out;

uniform mat4 _Projection;
uniform mat4 _View;
uniform mat4 _Transform;
uniform float _BackFace;

float GetCameraDot(vec3 voxelPoint, vec3 inputNormal)
{
    vec3 cameraEye = inverse(_View)[3].xyz;
    vec3 cameraToVoxel = (cameraEye - voxelPoint);
    return dot(cameraToVoxel, inputNormal);
}

bool ShouldRender(vec3 voxelPoint, vec3 inputNormal)
{
    bool backFaceRender = false;//bool(_BackFace);
    float cameraDot = GetCameraDot(voxelPoint, inputNormal);
    return backFaceRender ? (cameraDot <= 0.0f) : (cameraDot > 0.0f);
}

bool IsNeighbourActive(int bitcode, int index)
{
    return (bitcode & (1 << index)) != 0;
}

void main()
{
    mat4 projectionView = _Projection * _View;
    vec3 position = (_Transform * vec4(v_Out[0].position, 1.0f)).xyz;
    int neighbours = v_Out[0].neighbours;

    // top face
    if (!IsNeighbourActive(neighbours, 3) && ShouldRender(position, vec3(0.0f, 1.0f, 0.0f)))
    {
        g_Out.normal = vec3(0.0f, 1.0f, 0.0f);
        g_Out.textureId = v_Out[0].textures.z;
    
        g_Out.texcoord = vec2(0.0f, 0.0f);
        g_Out.position = position + vec3(-0.5f, 0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f, 0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 0.0f);
        g_Out.position = position + vec3( 0.5f, 0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3( 0.5f, 0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(0.0f, 1.0f);
        g_Out.position = position + vec3(-0.5f, 0.5f,  0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f, 0.5f,  0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 1.0f);
        g_Out.position = position + vec3( 0.5f, 0.5f,  0.5f);
        gl_Position = projectionView * vec4(position + vec3( 0.5f, 0.5f,  0.5f), 1.0f);
        EmitVertex();
    
        EndPrimitive();
    }

    // bottom face
    if (!IsNeighbourActive(neighbours, 2) && ShouldRender(position, vec3(0.0f, -1.0f, 0.0f)))
    {
        g_Out.normal = vec3(0.0f, -1.0f, 0.0f);
        g_Out.textureId = v_Out[0].textures.x;
        
        g_Out.texcoord = vec2(0.0f, 0.0f);
        g_Out.position = position + vec3(-0.5f, -0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f, -0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 0.0f);
        g_Out.position = position + vec3( 0.5f, -0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3( 0.5f, -0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(0.0f, 1.0f);
        g_Out.position = position + vec3(-0.5f, -0.5f,  0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f, -0.5f,  0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 1.0f);
        g_Out.position = position + vec3( 0.5f, -0.5f,  0.5f);
        gl_Position = projectionView * vec4(position + vec3( 0.5f, -0.5f,  0.5f), 1.0f);
        EmitVertex();
    
        EndPrimitive();
    }

    // right face
    if (!IsNeighbourActive(neighbours, 1) && ShouldRender(position, vec3(1.0f, 0.0f, 0.0f)))
    {
        g_Out.normal = vec3(1.0f, 0.0f, 0.0f);
        g_Out.textureId = v_Out[0].textures.y;
        
        g_Out.texcoord = vec2(0.0f, 0.0f);
        g_Out.position = position + vec3(0.5f, -0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3(0.5f, -0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 0.0f);
        g_Out.position = position + vec3(0.5f, -0.5f,  0.5f);
        gl_Position = projectionView * vec4(position + vec3(0.5f, -0.5f,  0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(0.0f, 1.0f);
        g_Out.position = position + vec3(0.5f,  0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3(0.5f,  0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 1.0f);
        g_Out.position = position + vec3(0.5f,  0.5f,  0.5f);
        gl_Position = projectionView * vec4(position + vec3(0.5f,  0.5f,  0.5f), 1.0f);
        EmitVertex();

        EndPrimitive();
    }

    // left face
    if (!IsNeighbourActive(neighbours, 0) && ShouldRender(position, vec3(-1.0f, 0.0f, 0.0f)))
    {
        g_Out.normal = vec3(-1.0f, 0.0f, 0.0f);
        g_Out.textureId = v_Out[0].textures.y;
        
        g_Out.texcoord = vec2(0.0f, 0.0f);
        g_Out.position = position + vec3(-0.5f, -0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f, -0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 0.0f);
        g_Out.position = position + vec3(-0.5f, -0.5f,  0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f, -0.5f,  0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(0.0f, 1.0f);
        g_Out.position = position + vec3(-0.5f,  0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f,  0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 1.0f);
        g_Out.position = position + vec3(-0.5f,  0.5f,  0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f,  0.5f,  0.5f), 1.0f);
        EmitVertex();

        EndPrimitive();
    }

    // back face
    if (!IsNeighbourActive(neighbours, 5) && ShouldRender(position, vec3(0.0f, 0.0f, 1.0f)))
    {
        g_Out.normal = vec3(0.0f, 0.0f, 1.0f);
        g_Out.textureId = v_Out[0].textures.y;
        
        g_Out.texcoord = vec2(0.0f, 0.0f);
        g_Out.position = position + vec3(-0.5f, -0.5f, 0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f, -0.5f, 0.5f), 1.0f);
        EmitVertex();

        g_Out.texcoord = vec2(1.0f, 0.0f);
        g_Out.position = position + vec3( 0.5f, -0.5f, 0.5f);
        gl_Position = projectionView * vec4(position + vec3( 0.5f, -0.5f, 0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(0.0f, 1.0f);
        g_Out.position = position + vec3(-0.5f,  0.5f, 0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f,  0.5f, 0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 1.0f);
        g_Out.position = position + vec3( 0.5f,  0.5f, 0.5f);
        gl_Position = projectionView * vec4(position + vec3( 0.5f,  0.5f, 0.5f), 1.0f);
        EmitVertex();

        EndPrimitive();
    }

    // front face
    if (!IsNeighbourActive(neighbours, 4) && ShouldRender(position, vec3(0.0f, 0.0f, -1.0f)))
    {
        g_Out.normal = vec3(0.0f, 0.0f, -1.0f);
        g_Out.textureId = v_Out[0].textures.y;
        
        g_Out.texcoord = vec2(0.0f, 0.0f);
        g_Out.position = position + vec3(-0.5f, -0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f, -0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 0.0f);
        g_Out.position = position + vec3( 0.5f, -0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3( 0.5f, -0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(0.0f, 1.0f);
        g_Out.position = position + vec3(-0.5f,  0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3(-0.5f,  0.5f, -0.5f), 1.0f);
        EmitVertex();
        
        g_Out.texcoord = vec2(1.0f, 1.0f);
        g_Out.position = position + vec3( 0.5f,  0.5f, -0.5f);
        gl_Position = projectionView * vec4(position + vec3( 0.5f,  0.5f, -0.5f), 1.0f);
        EmitVertex();

        EndPrimitive();
    }
}