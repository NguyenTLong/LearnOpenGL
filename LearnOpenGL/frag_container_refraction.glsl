#version 330 core


out vec4 FragColor;
uniform vec3 objectColor;
uniform vec3  viewPos;
uniform samplerCube skyBox;


in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

void main()
{
    float ratio = 1.00/ 1.52;
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skyBox, R).rgb, 1.0);
}

