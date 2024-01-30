#version 430 core

float AMBIENT = 0.1;

uniform vec3 cameraPos;

uniform vec3 color;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 spotlightPos;
uniform vec3 spotlightColor;
uniform vec3 spotlightConeDir;
uniform vec3 spotlightPhi;

uniform float exposition;

in vec3 vecNormal;
in vec3 worldPos;

in vec3 viewDirTS;
in vec3 lightDirTS;
in vec3 spotlightDirTS;
in vec3 test;

out vec4 outColor;

vec3 phongLight(vec3 lightDir, vec3 lightColor, vec3 normal,vec3 viewDir){
	float diffuse=max(0,dot(normal,lightDir));

	vec3 R = reflect(-lightDir, normal);  

	float specular = pow(max(dot(viewDir, R), 0.0), 32);

	vec3 resultColor = color*diffuse*lightColor+lightColor*specular;
	return resultColor;
}


void main()
{
	vec3 normal = vec3(0,0,1);
	vec3 viewDir = normalize(viewDirTS);
	vec3 lightDir = normalize(lightPos-worldPos);
	vec3 ambient = AMBIENT*color;
	vec3 attenuatedlightColor = lightColor/pow(length(lightPos-worldPos),2);
	vec3 ilumination;
	ilumination = ambient+phongLight(normalize(lightDirTS),attenuatedlightColor,normal,viewDir);
	//flashlight
	vec3 spotlightDir= normalize(spotlightPos-worldPos);
	float angle_atenuation = clamp((dot(-spotlightDir,spotlightConeDir)-0.8)*3,0,1);

	attenuatedlightColor = spotlightColor/pow(length(spotlightPos-worldPos),2)*angle_atenuation;
	ilumination=ilumination+phongLight(normalize(spotlightDirTS),attenuatedlightColor,normal,viewDir);
	
	outColor = vec4(1.0 - exp(-ilumination*exposition),1);
	//outColor = vec4(test,1);
}
