#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//shadow
uniform sampler2D shadowMap;

vec3 ambient;
vec3 diffuse;
vec3 specular;

float ambientStrength = 0.2f;
float specularStrength = 0.5f;
float shininess = 32.0f;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f); //in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

void luminaPunctiforma()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	

	vec3 lightPosEye = vec3(0.0f, 1.0f, 1.0f);

	float constant = 1.0f;
	float linear = 0.0045f;
	float quadratic = 0.0075f;

	float dist = length(lightPosEye - fPosEye.xyz);
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
	
	//compute light direction
	vec3 lightDirN = normalize(lightPosEye - fPosEye.xyz);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute ambient light
	ambient = att * ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = att * specularStrength * specCoeff * lightColor;
}

float computeShadows() 
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;

	if (normalizedCoords.z > 1.0f)
		return 0.0f;

	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;

	float bias = max(0.05f * (1.0f - dot(fNormal, lightDir)), 0.005f);
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	return shadow;
}



void main() 
{
	// computeLightComponents();
	luminaPunctiforma();
		
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	float shadow = computeShadows();
	vec3 color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);
    
    fColor = vec4(color, 1.0f);
}
