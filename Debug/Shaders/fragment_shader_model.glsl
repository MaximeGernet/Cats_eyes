/*
 * fragment_shader.glsl
 *
 *  Created on: Apr 8, 2016
 *      Author: root
 */

#version 330 core

//in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

//uniform sampler2D ourTexture1;
//uniform vec3 lightColor;
uniform vec3 viewPos;
//uniform sampler2D ourTexture2;

struct Material {
	int use_diff_map;
	int use_spec_map;
    sampler2D diffuse;
    sampler2D specular;
    vec3 diff_color;
    vec3 spec_color;
    float shininess;
};
uniform Material material;

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};
//uniform PointLight point_light;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
//uniform DirLight dir_light;

struct SceneLights {
	PointLight p_light[16];
	DirLight d_light[16];
	int p_light_cnt;
	int d_light_cnt;
};
uniform SceneLights scene_lights;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// Light effects	
	/*
	//float ambientStrength = 0.1f;
	//vec3 ambient = light.ambient * material.ambient;
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 result = 0;
	float diff = max(dot(norm, lightDir), 0.0);
	//vec3 diffuse = light.diffuse * (diff * material.diffuse);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
	//float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
	//vec3 specular = light.specular * (spec * material.specular);
	// Attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	result = ambient + diffuse + specular;
	*/
	int i;
	vec3 result = vec3(0.0f);
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	for(i = 0; i < scene_lights.p_light_cnt; i++)
	{
		result += CalcPointLight(scene_lights.p_light[i], normal, FragPos, viewDir);
	}
	for(i = 0; i < scene_lights.d_light_cnt; i++)
	{
		result += CalcDirLight(scene_lights.d_light[i], normal, viewDir);
	}
	
	color = vec4(result, 1.0f);// * vec4(ourColor, 1.0f); // * texture(ourTexture1, TexCoord);
	//color = vec4(scene_lights.d_light[0].ambient * vec3(texture(material.texture_diffuse1, TexCoord)), 1.0f);
	//color = vec4(1.0f);
	//color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.5);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Combine results
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	if(material.use_diff_map == 1)
	{
		ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
		diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));// * material.diff_color;
	}
	else
	{
		ambient = light.ambient * material.diff_color;
		diffuse = light.diffuse * diff * material.diff_color;
	}
	if(material.use_spec_map == 1)
	{
		specular = light.specular * spec * vec3(texture(material.specular, TexCoord));// * material.spec_color;
	}
	else
	{
		specular = light.specular * spec * material.spec_color;
	}
	return (ambient + diffuse + specular);
	//return ambient + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// Combine results
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	if(material.use_diff_map == 1)
	{
		ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
		diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));// * material.diff_color;
	}
	else
	{
		ambient = light.ambient * material.diff_color;
		diffuse = light.diffuse * diff * material.diff_color;
	}
	if(material.use_spec_map == 1)
	{
		specular = light.specular * spec * vec3(texture(material.specular, TexCoord));// * material.spec_color;
	}
	else
	{
		specular = light.specular * spec * material.spec_color;
	}
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
	//return specular;
}
