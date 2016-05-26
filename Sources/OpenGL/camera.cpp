/*
 * camera.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: root
 */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"

Camera_FPV::Camera_FPV(glm::vec3 _up, glm::vec3 _position, GLfloat _yaw, GLfloat _pitch, GLfloat _aspect_ratio)
{
	sensitivity = 0.05f;
	camera_speed = 3.0f;
	fov = 45.0f;
	up = _up;
	position = _position;
	yaw = _yaw;
	pitch = _pitch;
	aspect_ratio = _aspect_ratio;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(front);
}

void Camera_FPV::update(input_s* input, GLfloat delta_time)
{
	// Update camera direction
	yaw += sensitivity * input->mousexrel;
	pitch += sensitivity * input->mouseyrel;
	input->mousexrel = 0;
	input->mouseyrel = 0;

	if(pitch > 89.0f)
		pitch = 89.0f;
	if(pitch < -89.0f)
		pitch = -89.0f;

	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(front);

	// Update camera position
	if(input->key[GLFW_KEY_W])
		position += camera_speed * delta_time * front;
	if(input->key[GLFW_KEY_S])
		position -= camera_speed * delta_time * front;
	if(input->key[GLFW_KEY_A])
		position -= glm::normalize(glm::cross(front, up)) * camera_speed * delta_time;
	if(input->key[GLFW_KEY_D])
		position += glm::normalize(glm::cross(front, up)) * camera_speed * delta_time;
}

glm::mat4 Camera_FPV::view()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera_FPV::projection()
{
	return glm::perspective(glm::radians(fov), aspect_ratio, 0.1f, 100.0f);
}
