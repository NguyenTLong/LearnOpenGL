#version 330 core
#extension GL_ARB_separate_shader_objects: enable

layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
