/*
 * vertex.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: root
 */

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

#include "basics.h"
#include "shader.h"
#include "vertex.h"
#include "textures.h"
#include "material.h"

Triangle::Triangle(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
	vertices[0] = x1;
	vertices[1] = y1;
	vertices[2] = z1;
	vertices[3] = x2;
	vertices[4] = y2;
	vertices[5] = z2;
	vertices[6] = x3;
	vertices[7] = y3;
	vertices[8] = z3;
}

GLuint Triangle::size()
{
	return sizeof(vertices);
}

GLfloat* Triangle::data()
{
	return vertices;
}

Quad::Quad()
{
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 3;
	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;

	texture_cnt = 0;
}

void Quad::loadPosition(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3, GLfloat x4, GLfloat y4, GLfloat z4)
{
	int stride = 11;

	vertices[0 * stride + 0] = x1;
	vertices[0 * stride + 1] = y1;
	vertices[0 * stride + 2] = z1;
	vertices[1 * stride + 0] = x2;
	vertices[1 * stride + 1] = y2;
	vertices[1 * stride + 2] = z2;
	vertices[2 * stride + 0] = x3;
	vertices[2 * stride + 1] = y3;
	vertices[2 * stride + 2] = z3;
	vertices[3 * stride + 0] = x4;
	vertices[3 * stride + 1] = y4;
	vertices[3 * stride + 2] = z4;
}

void Quad::loadColor(GLfloat r1, GLfloat g1, GLfloat b1, GLfloat r2, GLfloat g2, GLfloat b2, GLfloat r3, GLfloat g3, GLfloat b3, GLfloat r4, GLfloat g4, GLfloat b4)
{
	int stride = 11;

	vertices[0 * stride + 3] = r1;
	vertices[0 * stride + 4] = g1;
	vertices[0 * stride + 5] = b1;
	vertices[1 * stride + 3] = r2;
	vertices[1 * stride + 4] = g2;
	vertices[1 * stride + 5] = b2;
	vertices[2 * stride + 3] = r3;
	vertices[2 * stride + 4] = g3;
	vertices[2 * stride + 5] = b3;
	vertices[3 * stride + 3] = r4;
	vertices[3 * stride + 4] = g4;
	vertices[3 * stride + 5] = b4;
}

void Quad::loadTexCoord(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
{
	int stride = 11;

	vertices[0 * stride + 6] = x1;
	vertices[0 * stride + 7] = y1;
	vertices[1 * stride + 6] = x2;
	vertices[1 * stride + 7] = y2;
	vertices[2 * stride + 6] = x3;
	vertices[2 * stride + 7] = y3;
	vertices[3 * stride + 6] = x4;
	vertices[3 * stride + 7] = y4;
}

void Quad::loadNormal(GLfloat nx, GLfloat ny, GLfloat nz)
{
	int stride = 11;

	vertices[0 * stride + 8] = nx;
	vertices[0 * stride + 9] = ny;
	vertices[0 * stride + 10] = nz;
	vertices[1 * stride + 8] = nx;
	vertices[1 * stride + 9] = ny;
	vertices[1 * stride + 10] = nz;
	vertices[2 * stride + 8] = nx;
	vertices[2 * stride + 9] = ny;
	vertices[2 * stride + 10] = nz;
	vertices[3 * stride + 8] = nx;
	vertices[3 * stride + 9] = ny;
	vertices[3 * stride + 10] = nz;
}

void Quad::assignTexture(GLuint _texture)
{
	texture[texture_cnt] = _texture;
	texture_cnt++;
}

void Quad::setMaterial(Texture _diffuse, Texture _specular, float _shininess)
{
	material.setMaterial(_diffuse, _specular, _shininess);
}

GLuint Quad::size()
{
	return sizeof(vertices);
}

GLfloat* Quad::data()
{
	return vertices;
}

void Quad::init()
{
	// Virtual array object initialization
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
		// Virtual buffer object initialization
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Element buffer object initialization
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,	GL_STATIC_DRAW);
		// Set the buffer attrbutes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
	glBindVertexArray(0);
}


void Quad::draw(GLuint _program)
{
	//glUseProgram(program);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glBindTexture(GL_TEXTURE_2D, texture[0]);
	
	if(texture_cnt < 2)
	{
		material.sendShaderData(_program);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glUniform1i(glGetUniformLocation(_program, "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glUniform1i(glGetUniformLocation(_program, "ourTexture2"), 1);
	}
	
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Quad::~Quad()
{
	glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

Line::Line(float _lat_org, float _lon_org)
{
	setOrg(_lat_org, _lon_org, 10);
}

void Line::setOrg(float _lat_org, float _lon_org, int _z)
{
	lat_org = _lat_org;
	lon_org = _lon_org;
	z = _z;

	x_ref = (lon_org + 180.0f) / 360.0f * pow(2.0f, z);
	//x_ref = floor(x_ref);
	y_ref = (1 - log(tan(lat_org * PI / 180.0f) + 1.0f / cos(lat_org * PI / 180.0f)) / PI) * pow(2.0f, z - 1);
	//y_ref = floor(y_ref);
	printf("x_ref: %f, y_ref: %f\n", x_ref, y_ref);

	indices.clear();
	vertices.clear();
}

void Line::init()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0],	GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Line::setPos(glm::vec2 _pos)
{
	pos = _pos;
}


void Line::setCoord(glm::vec2 _a, glm::vec2 _b)
{
	int stride = 4;
/*
	vertices[0 * stride + 0] = _a.x * screenWidth;
	vertices[0 * stride + 1] = _a.y * screenHeight;
	vertices[1 * stride + 0] = _b.x * screenWidth;
	vertices[1 * stride + 1] = _b.y * screenHeight;
	*/
}

void Line::addPoint(float lat, float lon, float _x_scale, float _y_scale)
{
	int stride = 4;

	float x, y;

	//x = lon;
	//y = asinh(tan(lat * PI / 180.0f));

	if(lat == 0.0f && lon == 0.0f)
		return;

	x = ((lon + 180.0f) / 360.0f * pow(2.0f, z));
	y = (1 - log(tan(lat * PI / 180.0f) + 1.0f / cos(lat * PI / 180.0f)) / PI) * pow(2.0f, z - 1);

	// x_rel and y_rel are coordinates between 0 and 1 (for a single tile), (0, 0) is the top left corner
	//float x_rel = x - floor(x);
	//float y_rel = 1.0f - (y - floor(y));

	float x_rel = x - x_ref;
	float y_rel = y - y_ref;

	//printf("x_rel: %0.2f, y_rel: %0.2f\n", x_rel, y_rel);
	/*
	if(pos.y * screenHeight + (1 - y_rel) * 256.0f < 10 )
	{
		printf("lat: %0.2f, lon: %0.2f\n", lat, lon);
		printf("x: %0.2f, y: %0.2f\n", pos.x * screenWidth + x_rel * 256.0f, pos.y * screenHeight + (1 - y_rel) * 256.0f);
	}
	*/

	vertices.push_back(x_rel * 256.0f * _x_scale);
	//vertices.push_back((1.0f - y_rel) * 256.0f);
	vertices.push_back((1.0f - y_rel * 256.0f * _y_scale / screenHeight) * screenHeight / _y_scale * _y_scale);
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);
	indices.push_back(indices.size());
}

void Line::draw(GLuint _program)
{
	glDisable(GL_DEPTH_TEST);
	glUniform1i(glGetUniformLocation(_program, "type"), LINE);
	glBindVertexArray(vao);
	glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

Line::~Line()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

Quad2D::Quad2D()
{
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 3;
	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;
}

void Quad2D::setPosition(glm::vec2 _pos, GLfloat _w, GLfloat _h)
{
	pos = _pos;
	w = _w;
	h = _h;
	setPosition();
}

void Quad2D::setPosition(glm::vec2 _pos)
{
	pos = _pos;
	setPosition();
}

void Quad2D::setPosition()
{
	int stride = 4;

	vertices[0 * stride + 0] = pos.x * screenWidth;
	vertices[0 * stride + 1] = pos.y * screenHeight;
	vertices[1 * stride + 0] = (pos.x + w) * screenWidth - 1.0f;
	vertices[1 * stride + 1] = pos.y * screenHeight;
	vertices[2 * stride + 0] = (pos.x + w) * screenWidth - 1.0f;
	vertices[2 * stride + 1] = (pos.y + h) * screenHeight;
	vertices[3 * stride + 0] = pos.x * screenWidth;
	vertices[3 * stride + 1] = (pos.y + h) * screenHeight;
}

void Quad2D::setTexCoord(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
{
	int stride = 4;

	vertices[0 * stride + 2] = x1;
	vertices[0 * stride + 3] = y1;
	vertices[1 * stride + 2] = x2;
	vertices[1 * stride + 3] = y2;
	vertices[2 * stride + 2] = x3;
	vertices[2 * stride + 3] = y3;
	vertices[3 * stride + 2] = x4;
	vertices[3 * stride + 3] = y4;
}


void Quad2D::setTexCoord(GLfloat _w, GLfloat _h, int _w_pow, int _h_pow)
{
	setTexCoord(0.0f, 0.0f, _w * screenWidth / _w_pow, 0.0f, _w * screenWidth / _w_pow, _h * screenHeight / _h_pow, 0.0f, _h * screenHeight / _h_pow);
}

void Quad2D::setTexCoord(glm::vec2 pos1, glm::vec2 pos2)
{
	setTexCoord(pos1.x, pos1.y, pos2.x, pos1.y, pos2.x, pos2.y, pos1.x, pos2.y);
}

void Quad2D::setTexture(unsigned char* _tex_data, int _w_pow, int _h_pow)
{
	texture.load(_tex_data, _w_pow, _h_pow);
}

void Quad2D::loadTexture(GLchar* _file_name)
{
	texture.load(_file_name, &w, &h);
}


void Quad2D::loadTexture(GLchar* _file_name, GLfloat _x_scale, GLfloat _y_scale)
{
	texture.load(_file_name, &w, &h);
	w *= _x_scale;
	h *= _y_scale;
}

void Quad2D::updateTexture(unsigned char* _tex_data, int _w_pow, int _h_pow)
{
	texture.update(_tex_data, _w_pow, _h_pow);
}

void Quad2D::updateTexture(GLchar* _file_name, GLfloat _x_scale, GLfloat _y_scale)
{
	texture.update(_file_name, &w, &h);
	w *= _x_scale;
	h *= _y_scale;
}
void Quad2D::init()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,	GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool Quad2D::isOverQuad(glm::vec2 _pos)
{
	if((_pos.x > pos.x) && (_pos.y > pos.y))
	{
		if((_pos.x < pos.x + w) && (_pos.y < pos.y + h))
		{
			return true;
		}
	}
	return false;
}

void Quad2D::draw(GLuint _program)
{
	glDisable(GL_DEPTH_TEST);
	glUniform1i(glGetUniformLocation(_program, "type"), TEXTURE_FROM_RGB_ARRAY);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id());
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}
/*
void Quad2D::draw(GLuint _program, GLfloat _w, GLfloat _h)
{
	if(_w == 0.0f)
	{

	}
	else if(_h == 0.0f)
	{

	}
}
*/
Quad2D::~Quad2D()
{
	glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

Cube::Cube(Texture _texture, glm::vec3 _position, GLfloat _scale)
{
	//model_p = glm::translate(glm::mat4(), _position);
	//_position.x = (-1.0f) * _position.x;

	quad[0].setMaterial(_texture, _texture, 32.0f);
	quad[1].setMaterial(_texture, _texture, 32.0f);
	quad[2].setMaterial(_texture, _texture, 32.0f);
	quad[3].setMaterial(_texture, _texture, 32.0f);
	quad[4].setMaterial(_texture, _texture, 32.0f);
	quad[5].setMaterial(_texture, _texture, 32.0f);

	quad[0].loadPosition( _position.x-_scale, _position.y-_scale, _position.z-_scale,
			_position.x + _scale, _position.y-_scale, _position.z-_scale,
			_position.x + _scale,  _position.y+_scale, _position.z-_scale,
			_position.x-_scale,  _position.y+_scale, _position.z-_scale);
	quad[0].loadNormal(0.0f, 0.0f, -1.0f);
	quad[1].loadPosition( _position.x-_scale, _position.y-_scale, _position.z+_scale,
			_position.x+_scale, _position.y-_scale, _position.z+_scale,
			_position.x+_scale,  _position.y+_scale, _position.z+_scale,
			_position.x-_scale,  _position.y+_scale, _position.z+_scale);
	quad[1].loadNormal(0.0f, 0.0f, 1.0f);
	quad[2].loadPosition( _position.x+_scale, _position.y-_scale, _position.z-_scale,
			_position.x+_scale, _position.y-_scale, _position.z+_scale,
			_position.x+_scale, _position.y+_scale, _position.z+_scale,
			_position.x+_scale, _position.y+_scale, _position.z-_scale);
	quad[2].loadNormal(1.0f, 0.0f, 0.0f);
	quad[3].loadPosition( _position.x-_scale, _position.y-_scale, _position.z-_scale,
			_position.x-_scale, _position.y-_scale, _position.z+_scale,
			_position.x-_scale, _position.y+_scale, _position.z+_scale,
			_position.x-_scale, _position.y+_scale, _position.z-_scale);
	quad[3].loadNormal(-1.0f, 0.0f, 0.0f);
	quad[4].loadPosition( _position.x-_scale, _position.y-_scale, _position.z-_scale,
			_position.x+_scale, _position.y-_scale, _position.z-_scale,
			_position.x+_scale, _position.y-_scale, _position.z+_scale,
			_position.x-_scale, _position.y-_scale, _position.z+_scale);
	quad[4].loadNormal(0.0f, -1.0f, 0.0f);
	quad[5].loadPosition( _position.x-_scale,  _position.y+_scale, _position.z-_scale,
			_position.x+_scale,  _position.y+_scale, _position.z-_scale,
			_position.x+_scale,  _position.y+_scale, _position.z+_scale,
			_position.x-_scale,  _position.y+_scale, _position.z+_scale);
	quad[5].loadNormal(0.0f, 1.0f, 0.0f);

	for(int i = 0; i < 6; i++)
	{
		quad[i].loadColor( -0.5f,  0.5f, 0.0f,
							0.5f, -0.5f, 0.0f,
							0.0f, 0.0f, 1.0f,
							0.0f, 1.0f, 1.0f);

		quad[i].loadTexCoord( 0.0f, 0.0f,
						   1.0f, 0.0f,
						   1.0f, 1.0f,
						   0.0f, 1.0f);
		quad[i].init();
		quad[i].assignTexture(_texture.id());
	}
}

void Cube::draw(GLuint _program)
{
	for(int i = 0; i < 6; i++)
		quad[i].draw(_program);
}

glm::mat4 Cube::model()
{
	return model_p;
}
