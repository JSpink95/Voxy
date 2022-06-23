#version 450 core

layout(location = 0) in vec3 a_Position;

uniform mat4 _LightSpaceMatrix;
uniform mat4 _Transform;

void main()
{
	gl_Position = _LightSpaceMatrix * _Transform * vec4(a_Position, 1.0f);
}