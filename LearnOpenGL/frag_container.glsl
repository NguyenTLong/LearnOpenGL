#version 330 core


out vec4 FragColor;
  
uniform vec3 objectColor;

uniform vec3  viewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
        
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
        
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

vec4 GetMatDiffuseColor();
vec4 GetMatSpecularColor();

vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight pointLight, vec3 normal,vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal,vec3 fragPos, vec3 viewDir);

void main()
{
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    // ambient
    vec3 result;
    result = CalcDirLight(dirLight, norm, viewDir);

//    for(int  i = 0; i < NR_POINT_LIGHTS; i++)
//    {
//        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
//    }
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    // diffuse
    FragColor = vec4(result, 1.0);
}

vec4 GetMatDiffuseColor()
{
    return texture(material.diffuse, TexCoords);
}

vec4 GetMatSpecularColor()
{
    return texture(material.specular, TexCoords);
}

vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir),  0.0), material.shininess);

    vec3 ambient = dirLight.ambient * GetMatDiffuseColor().rgb;
    vec3 diffuse = dirLight.diffuse * diff * GetMatDiffuseColor().rgb;
    vec3 specular = dirLight.specular * spec * GetMatSpecularColor().rgb;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(pointLight.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float dis = length(pointLight.position - fragPos);
    float attenuation = 1.0/ (pointLight.constant + pointLight.linear * dis + pointLight.quadratic * (dis * dis));

    vec3 ambient = pointLight.ambient * GetMatDiffuseColor().rgb;
    vec3 diffuse = pointLight.diffuse * diff * GetMatDiffuseColor().rgb;
    vec3 specular = pointLight.specular * spec * GetMatSpecularColor().rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal,vec3 fragPos, vec3 viewDir)
{
    
    vec3 lightDir = normalize(spotLight.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float dis = length(spotLight.position - fragPos);
    float attenuation = 1.0/ (spotLight.constant + spotLight.linear * dis + spotLight.quadratic * (dis * dis));


    float theta = dot(lightDir, normalize(-spotLight.direction));
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    

    vec3 ambient = spotLight.ambient * GetMatDiffuseColor().rgb;
    vec3 diffuse = spotLight.diffuse * diff * GetMatDiffuseColor().rgb;
    vec3 specular = spotLight.specular * spec * GetMatSpecularColor().rgb;
    
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular);
}
