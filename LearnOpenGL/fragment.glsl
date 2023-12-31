#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;

uniform vec3 lightPos;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
//	FragColor = mix(texture(texture1, TexCoord),texture(texture2, TexCoord), 0.5) * vec4(ourColor, 1.0);
	FragColor = texture(texture1, TexCoord);
}