/*
 * material.h
 *
 *  Created on: Apr 14, 2016
 *      Author: root
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "textures.h"

class Material
{
private:
	Texture diffuse;	// ambient map = diffuse map
	Texture specular;
	glm::vec3 diff_color;
	glm::vec3 spec_color;
	float shininess;
	int use_diff_map;
	int use_spec_map;

public:
	Material(Texture _diffuse, Texture _specular, float _shininess);
	Material();
	void setMaterial(Texture _diffuse, Texture _specular, float _shininess);
	void setDiffuseMap(Texture _diffuse);
	void setSpecularMap(Texture _specular, float _shininess);
	void setDiffuseColor(GLfloat _r, GLfloat _g, GLfloat _b);
	void setSpecularColor(GLfloat _r, GLfloat _g, GLfloat _b, float _shininess);
	//void load(aiMaterial* mat, aiTextureType type, std::string typeName);
	void sendShaderData(GLuint _program);
};

#endif /* MATERIAL_H_ */
