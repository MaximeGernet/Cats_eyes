/*
 * vertex.h
 *
 *  Created on: Apr 8, 2016
 *      Author: root
 */

#ifndef VERTEX_H_
#define VERTEX_H_

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "textures.h"
#include "material.h"


struct Vertex_s {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Triangle
{
private:
	GLfloat vertices[9];
	GLuint vao;
	GLuint vbo;

public:
	Triangle(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3);
	GLuint size();
	GLfloat* data();
};

class Quad
{
private:
	GLfloat vertices[44]; // Contains the position, the color, the texture coordinates and the normal
	GLuint indices[6];
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint texture[16];
	Material material;

	char texture_cnt;

public:
	Quad();
	~Quad();

	void loadPosition(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3, GLfloat x4, GLfloat y4, GLfloat z4);
	void loadColor(GLfloat r1, GLfloat g1, GLfloat b1, GLfloat r2, GLfloat g2, GLfloat b2, GLfloat r3, GLfloat g3, GLfloat b3, GLfloat r4, GLfloat g4, GLfloat b4);
	void loadTexCoord(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
	void loadNormal(GLfloat nx, GLfloat ny, GLfloat nz);
	void assignTexture(GLuint _texture);
	void setMaterial(Texture _diffuse, Texture _specular, float _shininess);

	GLuint size();
	GLfloat* data();

	void init();
	void draw(GLuint _program);
};

class Quad2D
{
private:
	GLfloat vertices[16]; // Contains the position and the texture coordinates
	GLuint indices[6];
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	Texture texture;
	glm::vec2 pos;
	GLfloat w, h;

public:
	Quad2D();
	~Quad2D();

	void setPosition(glm::vec2 _pos, GLfloat _w, GLfloat _h);
	void setPosition(glm::vec2 _pos);
	void setPosition();
	void setTexCoord(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
	void setTexCoord(GLfloat _w, GLfloat _h, int _w_pow, int _h_pow);
	void setTexCoord(glm::vec2 pos1, glm::vec2 pos2);
	void setTexture(unsigned char* _tex_data, int _w_pow, int _h_pow);
	void loadTexture(GLchar* _file_name);
	void loadTexture(GLchar* _file_name, GLfloat _x_scale, GLfloat _y_scale);
	void updateTexture(unsigned char* _tex_data, int _w_pow, int _h_pow);
	void updateTexture(GLchar* _file_name, GLfloat _x_scale, GLfloat _y_scale);
	void init();

	bool isOverQuad(glm::vec2 _pos);
	void draw(GLuint _program);
	//void draw(GLuint _program, GLfloat _w, GLfloat _h);
};

class Line
{
public:
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	glm::vec2 pos;

	int z;
	float lat_org, lon_org;
	float x_ref, y_ref;

	Line(float _lat_org = 0.0f, float _lon_org = 0.0f);
	~Line();
	void init();
	void setPos(glm::vec2 _pos);
	void setOrg(float _lat_org, float _lon_org, int _z);
	void setCoord(glm::vec2 _a, glm::vec2 _b);
	void addPoint(float lat, float lon, float _x_scale, float _y_scale);
	void draw(GLuint _program);
};

class Cube
{
private:
	Quad quad[6];
	glm::mat4 model_p;

public:
	Cube(Texture _texture, glm::vec3 _position, GLfloat _scale);
	glm::mat4 model();
	void draw(GLuint _program);
};

/*
class MyMesh
{
private:
	GLfloat* vetices;
	GLuint* indices;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint texture[16];
	GLuint program;

	char texture_cnt;

public:
	MyMesh();
	~MyMesh();

	void loadPosition(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3, GLfloat x4, GLfloat y4, GLfloat z4);
	void loadColor(GLfloat r1, GLfloat g1, GLfloat b1, GLfloat r2, GLfloat g2, GLfloat b2, GLfloat r3, GLfloat g3, GLfloat b3, GLfloat r4, GLfloat g4, GLfloat b4);
	void loadTexCoord(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
	void assignTexture(GLuint _texture);
	void assignProgram(GLuint _program);

	GLuint size();
	GLfloat* data();

	void init();
	void draw();
};
*/

#endif /* VERTEX_H_ */
