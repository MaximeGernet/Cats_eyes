/*
 * textures.h
 *
 *  Created on: Apr 8, 2016
 *      Author: root
 */

#ifndef TEXTURES_H_
#define TEXTURES_H_

//#define GLEW_STATIC
#include <GL/glew.h>

#include <assimp/scene.h>

#include <string>

// Shader options
#define TEXTURE_FROM_FONT		1
#define TEXTURE_FROM_RGB_ARRAY	2
#define LINE					3

class Texture
{
private:
	GLuint tex_id;
	aiString tex_path; // We store the path of the texture to compare with other textures
	unsigned char* tex_data;
	int width, height;

	void genTexture();
	void setTexData(unsigned char* _tex_data, int _w_pow, int _h_pow);

public:
	Texture(const GLchar* _file_path);
	Texture();
	~Texture();
	void load(const GLchar* _file_path, GLfloat* _w, GLfloat* _h);
	void load(const GLchar* _file_path);
	void load(unsigned char* _tex_data, int _w_pow, int _h_pow);
	void update(unsigned char* _tex_data, int _w_pow, int _h_pow);
	void update(const GLchar* _file_path);
	void update(const GLchar* _file_path, GLfloat* _w, GLfloat* _h);
	void setPath(aiString _path);
	void bind(GLuint _program, char _texture_unit, const char* _sampler_name);
	aiString path();
	GLuint id();
};

void addFrame(GLubyte* tex, GLfloat w, GLfloat h, int w_power);

void createDot(int x_coord, int y_coord, int size, GLubyte* tex_data, int w_pixel, int w_power);

#endif /* TEXTURES_H_ */
