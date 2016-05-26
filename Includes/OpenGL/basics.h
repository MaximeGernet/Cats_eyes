/*
 * basics.h
 *
 *  Created on: Apr 19, 2016
 *      Author: root
 */

#ifndef BASICS_H_
#define BASICS_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define FPS 60.0f
#define nullptr NULL
#define PI 3.14159265f

extern GLfloat screenWidth;
extern GLfloat screenHeight;

void sleep_millis(int64_t millis);

float dist(glm::vec2 a, glm::vec2 b);

#endif /* BASICS_H_ */
