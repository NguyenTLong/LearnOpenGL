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
uniform sampler2D shadowMap;

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

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

vec4 GetMatDiffuseColor();
vec4 GetMatSpecularColor();

vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight pointLight, vec3 normal,vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal,vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main()
{
    
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // ambient
    vec3 result;
    result = CalcDirLight(dirLight, norm, viewDir);

//    for(int  i = 0; i < NR_POINT_LIGHTS; i++)
//    {
//        result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
//    }
//    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    // diffuse

    FragColor = vec4(result, 1.0);
//    FragColor = vec4(vec3(gl_FragCoord.z),1.0);
}

vec4 GetMatDiffuseColor()
{
    return texture(material.diffuse, fs_in.TexCoords);
}

vec4 GetMatSpecularColor()
{
    return texture(material.specular, fs_in.TexCoords);
}

vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfVector = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfVector), 0.0), material.shininess);

    vec3 ambient = dirLight.ambient * GetMatDiffuseColor().rgb;
    vec3 diffuse = dirLight.diffuse * diff * GetMatDiffuseColor().rgb;
    vec3 specular = dirLight.specular * spec * GetMatSpecularColor().rgb;

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, fs_in.Normal, lightDir);
//    return (ambient + (1.0 - shadow) * (diffuse + specular));
//    return (ambient + diffuse + specular);
    return vec3(shadow);
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(pointLight.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 halfVector = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfVector), 0.0), material.shininess);

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

    vec3 halfVector = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfVector), 0.0), material.shininess);

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

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   // remap from -1,1 to 0,1 
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture( shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float shadowBias = max(.05 * (0.1 - dot(normal, lightDir)), 0.005);
    float shadow = currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;
    return shadow;

}
