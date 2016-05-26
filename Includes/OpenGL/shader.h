/*
 * shader.h
 *
 *  Created on: Apr 8, 2016
 *      Author: root
 */

#ifndef SHADER_H_
#define SHADER_H_

using namespace std;

#include <GL/glew.h>

class Shader
{
public:
	// The program ID
	GLuint Program;
	// Constructor reads and builds the shader
	Shader(const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath);

	GLuint id();
	// Use the program
	void Use();
};


#endif /* SHADER_H_ */
