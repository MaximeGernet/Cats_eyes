/*
 * events.h
 *
 *  Created on: Apr 8, 2016
 *      Author: root
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef struct
{
	int key[GLFW_KEY_LAST + 1];
	GLfloat mousex,mousey;
	GLfloat mousexrel, mouseyrel;
	int scrolly;
	char mousebuttons[GLFW_MOUSE_BUTTON_LAST + 1];
	char quit;
	char init;
} input_s;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

bool isOver(glm::vec2 pos, float w, float h, input_s* input);

#endif /* EVENTS_H_ */
