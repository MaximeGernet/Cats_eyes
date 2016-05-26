/*
 * events.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: root
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

#include "basics.h"
#include "events.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	input_s* input = reinterpret_cast<input_s *>(glfwGetWindowUserPointer(window));

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		input->quit = 1;
	}

	if(action == GLFW_PRESS)
	{
		input->key[key] = 1;
	}

	if(action == GLFW_RELEASE)
	{
		input->key[key] = 0;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	input_s* input = reinterpret_cast<input_s *>(glfwGetWindowUserPointer(window));
	if(input->init)
	{
		input->init = 0;
		input->mousex = xpos;
		input->mousey = ypos;
		input->mousexrel = 0;
		input->mouseyrel = 0;
		return;
	}
	input->mousexrel += xpos - input->mousex;
	input->mouseyrel += input->mousey - ypos;
	input->mousex = xpos;
	input->mousey = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	input_s* input = reinterpret_cast<input_s *>(glfwGetWindowUserPointer(window));

	if(action == GLFW_PRESS)
	{
		input->mousebuttons[button] = 1;
	}

	if(action == GLFW_RELEASE)
	{
		input->mousebuttons[button] = 0;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	input_s* input = reinterpret_cast<input_s *>(glfwGetWindowUserPointer(window));

	input->scrolly += (int)yoffset;

	//printf("scrolly: %d\n", input->scrolly);
}

bool isOver(glm::vec2 pos, float w, float h, input_s* input)
{
	float mx = input->mousex / screenWidth;
	float my = 1.0 - input->mousey / screenHeight;

	if(mx > pos.x && my > pos.y)
	{
		if(mx < pos.x + w && my < pos.y + h)
		{
			return true;
		}
	}
	return false;
}
