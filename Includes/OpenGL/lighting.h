/*
 * lighting.h
 *
 *  Created on: Apr 12, 2016
 *      Author: root
 */

#ifndef LIGHTING_H_
#define LIGHTING_H_

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class Point_light
{
private:
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	// Attenuation
	float constant;
	float linear;
	float quadratic;

public:
	Point_light(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
	Point_light();
	void setIntensity(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
	void setPosition(glm::vec3 _position);
	void setAttenuation(float _constant, float _linear, float _quadratic);
	void sendShaderData(GLuint _program);
	void sendShaderData(GLuint _program, char _i);
};

class Directional_light
{
private:
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

public:
	Directional_light(glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
	Directional_light();
	void setIntensity(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
	void setDirection(glm::vec3 _direction);
	void sendShaderData(GLuint _program);
	void sendShaderData(GLuint _program, char _i);
};

class Scene_lights
{
private:
	Point_light p_light[16];
	Directional_light d_light[16];
	char p_light_cnt;
	char d_light_cnt;

public:
	Scene_lights();
	void newDirectionalLight(glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
	void newPointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
	void sendShaderData(GLuint _program);
};


#endif /* LIGHTING_H_ */
