#version 330 core
#extension GL_ARB_separate_shader_objects: enable
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = aPos;
	gl_Position = (projection * view * vec4(aPos, 1.0)).xyww;
};