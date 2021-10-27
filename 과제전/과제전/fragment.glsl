#version 330 core

in vec3 v2f_normal;
in vec3 v2f_worldPos; 
in vec2 TexCoord;

out vec4 out_color;

uniform sampler2D outTexture;
uniform vec3 g_lightAmbient;
uniform vec3 g_lightPos;
uniform vec3 g_lightColor;
uniform vec3 g_objectColor;
uniform vec3 g_cameraPos;
uniform int flag;

void main(void)
{	
	vec3 ambient = vec3(0.5,0.5,0.5) * g_lightColor;

	vec3 normalVector = normalize(v2f_normal);
	vec3 lightDir = normalize(g_lightPos - v2f_worldPos);
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * g_lightColor;

	int shininess = 128;
	vec3 viewDir = normalize(g_cameraPos - v2f_worldPos);
	vec3 reflectDir = reflect(-lightDir, normalVector);
	float specularLight = max(dot(viewDir, reflectDir), 0.0);
	specularLight = pow(specularLight, shininess);
	vec3 specular = specularLight * g_lightColor;

	vec3 result = (ambient + diffuse + specular) * g_objectColor;
	
	out_color = vec4(result, 1.0);
	if (flag == 1) 
		out_color = texture(outTexture, TexCoord) * vec4(g_objectColor, 1.0);
	else if (flag == 2)
		out_color = texture(outTexture, TexCoord) * vec4(result, 1.0);

}