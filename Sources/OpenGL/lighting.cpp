/*
 * lighting.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: root
 */

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lighting.h"

Point_light::Point_light(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
{
	setIntensity(_ambient, _diffuse, _specular);
	setPosition(_position);
	setAttenuation(1.0f, 0.0f, 0.0f);
}

Point_light::Point_light()
{

}

void Point_light::setIntensity(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
{
	ambient = _ambient;
	diffuse = _diffuse;
	specular = _specular;
}

void Point_light::setPosition(glm::vec3 _position)
{
	position = _position;
}

void Point_light::setAttenuation(float _constant, float _linear, float _quadratic)
{
	constant = _constant;
	linear = _linear;
	quadratic = _quadratic;
}

void Point_light::sendShaderData(GLuint _program)
{
	glUniform3f(glGetUniformLocation(_program, "light.ambient"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(_program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(_program, "light.specular"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(_program, "light.position"), position.x, position.y, position.z);

	glUniform1f(glGetUniformLocation(_program, "light.constant"), constant);
	glUniform1f(glGetUniformLocation(_program, "light.linear"), linear);
	glUniform1f(glGetUniformLocation(_program, "light.quadratic"), quadratic);
}

void Point_light::sendShaderData(GLuint _program, char _i)
{
	char uniform_name[64];
	sprintf(uniform_name, "scene_lights.p_light[%u].ambient", _i);
	glUniform3f(glGetUniformLocation(_program, uniform_name), ambient.r, ambient.g, ambient.b);
	sprintf(uniform_name, "scene_lights.p_light[%u].diffuse", _i);
	glUniform3f(glGetUniformLocation(_program, uniform_name), diffuse.r, diffuse.g, diffuse.b);
	sprintf(uniform_name, "scene_lights.p_light[%u].specular", _i);
	glUniform3f(glGetUniformLocation(_program, uniform_name), specular.r, specular.g, specular.b);
	sprintf(uniform_name, "scene_lights.p_light[%u].position", _i);
	glUniform3f(glGetUniformLocation(_program, uniform_name), position.x, position.y, position.z);

	sprintf(uniform_name, "scene_lights.p_light[%u].constant", _i);
	glUniform1f(glGetUniformLocation(_program, uniform_name), constant);
	sprintf(uniform_name, "scene_lights.p_light[%u].linear", _i);
	glUniform1f(glGetUniformLocation(_program, uniform_name), linear);
	sprintf(uniform_name, "scene_lights.p_light[%u].quadratic", _i);
	glUniform1f(glGetUniformLocation(_program, uniform_name), quadratic);
}

Directional_light::Directional_light(glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
{
	setIntensity(_ambient, _diffuse, _specular);
	setDirection(_direction);
}

Directional_light::Directional_light()
{

}

void Directional_light::setIntensity(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
{
	ambient = _ambient;
	diffuse = _diffuse;
	specular = _specular;
}

void Directional_light::setDirection(glm::vec3 _direction)
{
	direction = _direction;
}

void Directional_light::sendShaderData(GLuint _program)
{
	glUniform3f(glGetUniformLocation(_program, "light.ambient"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(_program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(_program, "light.specular"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(_program, "light.direction"), direction.x, direction.y, direction.z);
}

void Directional_light::sendShaderData(GLuint _program, char _i)
{
	char uniform_name[64];
	sprintf(uniform_name, "scene_lights.d_light[%u].ambient", _i);
	glUniform3f(glGetUniformLocation(_program, uniform_name), ambient.r, ambient.g, ambient.b);
	sprintf(uniform_name, "scene_lights.d_light[%u].diffuse", _i);
	glUniform3f(glGetUniformLocation(_program, uniform_name), diffuse.r, diffuse.g, diffuse.b);
	sprintf(uniform_name, "scene_lights.d_light[%u].specular", _i);
	glUniform3f(glGetUniformLocation(_program, uniform_name), specular.r, specular.g, specular.b);
	sprintf(uniform_name, "scene_lights.d_light[%u].direction", _i);
	glUniform3f(glGetUniformLocation(_program, uniform_name), direction.x, direction.y, direction.z);
}

Scene_lights::Scene_lights()
{
	p_light_cnt = 0;
	d_light_cnt = 0;
}

void Scene_lights::newDirectionalLight(glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
{
	d_light[d_light_cnt].setIntensity(_ambient, _diffuse, _specular);
	d_light[d_light_cnt].setDirection(_direction);
	d_light_cnt++;
}

void Scene_lights::newPointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
{
	p_light[p_light_cnt].setIntensity(_ambient, _diffuse, _specular);
	p_light[p_light_cnt].setPosition(_position);
	p_light[p_light_cnt].setAttenuation(1.0f, 0.14f, 0.07f);
	p_light_cnt++;
}

void Scene_lights::sendShaderData(GLuint _program)
{
	glUniform1i(glGetUniformLocation(_program, "scene_lights.p_light_cnt"), p_light_cnt);
	glUniform1i(glGetUniformLocation(_program, "scene_lights.d_light_cnt"), d_light_cnt);
	for(int i = 0; i < p_light_cnt; i++)
	{
		p_light[i].sendShaderData(_program, i);
	}
	for(int i = 0; i < d_light_cnt; i++)
	{
		d_light[i].sendShaderData(_program, i);
	}
}
