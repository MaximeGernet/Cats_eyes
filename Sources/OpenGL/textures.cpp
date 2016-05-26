/*
 * textures.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: root
 */

#define GLEW_STATIC
#include <GL/glew.h>
#include <SOIL/SOIL.h>

#include <string>

#include "basics.h"
#include "textures.h"

Texture::Texture(const GLchar* _file_path)
{
	GLfloat w, h;
	tex_data = NULL;
	load(_file_path, &w, &h);
}

Texture::Texture()
{
	tex_data = NULL;
}

Texture::~Texture()
{

}

void Texture::load(const GLchar* _file_path, GLfloat* _w, GLfloat* _h)
{
	tex_data = SOIL_load_image(_file_path, &width, &height, 0, SOIL_LOAD_RGBA);
	if( tex_data == 0 )
	{
		printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
	}
	*_w = (GLfloat)width / screenWidth;
	*_h = (GLfloat)height / screenHeight;
	genTexture();
}


void Texture::load(const GLchar* _file_path)
{
	tex_data = SOIL_load_image(_file_path, &width, &height, 0, SOIL_LOAD_RGBA);
	genTexture();
}

void Texture::load(unsigned char* _tex_data, int _w_pow, int _h_pow)
{
	setTexData(_tex_data, _w_pow, _h_pow);
	genTexture();
}

void Texture::update(unsigned char* _tex_data, int _w_pow, int _h_pow)
{
	//glDeleteTextures(1, &tex_id);
	setTexData(_tex_data, _w_pow, _h_pow);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::update(const GLchar* _file_path)
{
	glDeleteTextures(1, &tex_id);
	tex_data = SOIL_load_image(_file_path, &width, &height, 0, SOIL_LOAD_RGBA);
	genTexture();
	/*
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	glBindTexture(GL_TEXTURE_2D, 0);
	*/
}

void Texture::update(const GLchar* _file_path, GLfloat* _w, GLfloat* _h)
{
	glDeleteTextures(1, &tex_id);
	SOIL_free_image_data(tex_data);
	tex_data = SOIL_load_image(_file_path, &width, &height, 0, SOIL_LOAD_RGBA);
	if( tex_data == 0 )
	{
		printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
		tex_data = SOIL_load_image("Maps/black.png", &width, &height, 0, SOIL_LOAD_RGBA);
	}
	*_w = (GLfloat)width / screenWidth;
	*_h = (GLfloat)height / screenHeight;
	genTexture();
}

void Texture::setPath(aiString _path)
{
	tex_path = _path;
}

void Texture::setTexData(unsigned char* _tex_data, int _w_pow, int _h_pow)
{
	tex_data = _tex_data;
	width = _w_pow;
	height = _h_pow;
}

void Texture::genTexture()
{
	glGenTextures(1, &tex_id);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	//glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(GLuint _program, char _texture_unit, const char* _sampler_name)
{
	glActiveTexture(GL_TEXTURE0 + _texture_unit);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glUniform1i(glGetUniformLocation(_program, _sampler_name), 0);
}

GLuint Texture::id()
{
	return tex_id;
}

aiString Texture::path()
{
	return tex_path;
}

void addFrame(GLubyte* tex, GLfloat w, GLfloat h, int w_power)
{
	int i;
	int w_pixel = w * screenWidth;
	int h_pixel = h * screenHeight;
	for(i = 0; i < w_pixel; i++)
	{
		tex[0 + (w_power * 0 + i) * 4] = 150;
		tex[1 + (w_power * 0 + i) * 4] = 150;
		tex[2 + (w_power * 0 + i) * 4] = 150;
		tex[3 + (w_power * 0 + i) * 4] = 255;
	}
	for(i = 0; i < w_pixel; i++)
	{
		tex[0 + (w_power * (h_pixel - 1) + i) * 4] = 150;
		tex[1 + (w_power * (h_pixel - 1) + i) * 4] = 150;
		tex[2 + (w_power * (h_pixel - 1) + i) * 4] = 150;
		tex[3 + (w_power * (h_pixel - 1) + i) * 4] = 255;
	}
	for(i = 0; i < h_pixel; i++)
	{
		tex[0 + (w_power * i + 0) * 4] = 150;
		tex[1 + (w_power * i + 0) * 4] = 150;
		tex[2 + (w_power * i + 0) * 4] = 150;
		tex[3 + (w_power * i + 0) * 4] = 255;
	}
	for(i = 0; i < h_pixel; i++)
	{
		tex[0 + (w_power * i + (w_pixel - 1)) * 4] = 150;
		tex[1 + (w_power * i + (w_pixel - 1)) * 4] = 150;
		tex[2 + (w_power * i + (w_pixel - 1)) * 4] = 150;
		tex[3 + (w_power * i + (w_pixel - 1)) * 4] = 255;
	}
}

void createDot(int x_coord, int y_coord, int size, GLubyte* tex_data, int w_pixel, int w_power)
{
	int i, j;
	for(i = y_coord - size + 1; i < y_coord + size; i++)
	{
		for(j = x_coord - size + 1; j < x_coord + size; j++)
		{
			tex_data[0 + (w_power * i + j) * 4] = 255;
			tex_data[1 + (w_power * i + j) * 4] = 255;
			tex_data[2 + (w_power * i + j) * 4] = 255;
			tex_data[3 + (w_power * i + j) * 4] = 255;
		}
	}
}
