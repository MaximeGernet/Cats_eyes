/*
 * camera.h
 *
 *  Created on: Apr 12, 2016
 *      Author: root
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "events.h"

class Camera_FPV
{
private:
	glm::vec3 front;
	glm::vec3 up;
	GLfloat aspect_ratio;
	GLfloat pitch;
	GLfloat sensitivity;
	GLfloat camera_speed;
	GLfloat fov;

public:
	glm::vec3 position;
	GLfloat yaw;
	Camera_FPV(glm::vec3 _up, glm::vec3 _position, GLfloat _yaw, GLfloat _pitch, GLfloat _aspect_ratio);
	void update(input_s* input, GLfloat delta_time);
	glm::mat4 view();
	glm::mat4 projection();
};

class Camera_3D
{
private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 target;

public:
	Camera_3D();
	void update();
	glm::mat4 view();
};

#endif /* CAMERA_H_ */
