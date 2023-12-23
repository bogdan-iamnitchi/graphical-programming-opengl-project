#version 410 core

//------------------inputs------------------//
//for loading model
in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
//for shadows
in vec4 fragPosLightSpace;

//------------------outputs------------------//
//output a final color for the fragment
out vec4 fColor;

//------------------structs------------------//
struct Material {

    float shininess;
}; 

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

//------------------uniforms------------------//
//transformations
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//materials
uniform Material material;

//lighting
uniform DirLight dirLight;
uniform PointLight pointLight;

//shadow mapping
uniform sampler2D shadowMap;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//control variables
uniform int activateDirLight;
uniform int activatePointLight;
uniform int activateShadows;
uniform int activateFog;

vec3 finalColor = vec3(0.0f);

vec3 computeShadow(DirLight light, vec3 ambient, vec3 diffuse, vec3 specular)
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	
	if (normalizedCoords.z > 1.0f) 
        return vec3(ambient + diffuse + specular);

	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;

	float bias = max(0.05f * (1.0f - dot(fNormal, light.direction)), 0.05f);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;	

    vec3 colorWithShadows = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);

	return colorWithShadows;
}

vec3 computeDirLightWithShadows(DirLight light, sampler2D diffuseTexture, sampler2D specularTexture)
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //compute ambient light
    vec3 ambient = light.ambient * texture(diffuseTexture, fTexCoords).rgb;

    //compute diffuse light
    vec3 lightDirN = vec3(normalize(view * vec4(light.direction, 0.0f)));
    float diffCoeff = max(dot(normalEye, lightDirN), 0.0f);
	vec3 diffuse = light.diffuse * (diffCoeff * texture(diffuseTexture, fTexCoords).rgb);

    //compute specular light
    vec3 viewDirN = normalize(-fPosEye.xyz);
    vec3 halfVector = normalize(lightDirN + viewDirN);
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), material.shininess);
    vec3 specular = light.specular * (specCoeff * texture(specularTexture, fTexCoords).rgb);

    if (activateShadows == 1)
        return computeShadow(light, ambient, diffuse, specular);
    
    return min((ambient + diffuse + specular), 1.0f);
}

vec3 computePointLight(PointLight light, sampler2D diffuseTexture, sampler2D specularTexture)
{
    //compute eye space coordinates
    vec4 fPosEye = vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(fNormal);

    //compute ambient light
    vec3 ambient = light.ambient * texture(diffuseTexture, fTexCoords).rgb;

    //compute diffuse light
    vec3 lightDirN = normalize(light.position - fPosEye.xyz);
    float diffCoeff = max(dot(normalEye, lightDirN), 0.0f);
	vec3 diffuse = light.diffuse * (diffCoeff * texture(diffuseTexture, fTexCoords).rgb);

    //compute specular light
    vec3 viewDirN = normalize(light.position - fPosEye.xyz);
    vec3 halfVector = normalize(lightDirN + viewDirN);
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), material.shininess);
    vec3 specular = light.specular * (specCoeff * texture(specularTexture, fTexCoords).rgb);

    //calcualte distance from light to fragment and attenation
	float distance = length(light.position - fPosEye.xyz);
	float attenation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //attenuate each component
    ambient *= attenation;
    diffuse *= attenation;
    specular *= attenation;

    return min((ambient + diffuse + specular), 1.0f);
}

float computeFog()
{
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);

    float fogDensity = 0.009f;
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

    return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
    vec3 dirColor = computeDirLightWithShadows(dirLight, diffuseTexture, specularTexture);
    vec3 pointColor = computePointLight(pointLight, diffuseTexture, specularTexture);

    activateDirLight == 1 ? finalColor += dirColor : finalColor += dirLight.ambient * 0.1 * texture(diffuseTexture, fTexCoords).rgb;
    activatePointLight == 1 ? finalColor += pointColor : finalColor += vec3(0.0f);

    if(activateFog == 1){
        float fogFactor = computeFog();
        vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        fColor = mix(fogColor, vec4(finalColor, 1.0f), fogFactor);
    }
    else {
        //output final color
        fColor = vec4(finalColor, 1.0f);
    }

}
