/*
 * material.cpp
 *
 *  Created on: Apr 14, 2016
 *      Author: root
 */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "textures.h"
#include "material.h"

Material::Material(Texture _diffuse, Texture _specular, float _shininess)
{
	use_spec_map = 1;
	use_diff_map = 1;
	diffuse = _diffuse;
	specular = _specular;
	shininess = _shininess;
}

Material::Material()
{
	use_spec_map = 0;
	use_diff_map = 0;
}

void Material::setMaterial(Texture _diffuse, Texture _specular, float _shininess)
{
	use_spec_map = 1;
	use_diff_map = 1;
	diffuse = _diffuse;
	specular = _specular;
	shininess = _shininess;
}


void Material::setDiffuseMap(Texture _diffuse)
{
	use_diff_map = 1;
	diffuse = _diffuse;
}

void Material::setSpecularMap(Texture _specular, float _shininess)
{
	use_spec_map = 1;
	specular = _specular;
	shininess = _shininess;
}

void Material::setDiffuseColor(GLfloat _r, GLfloat _g, GLfloat _b)
{
	diff_color = glm::vec3(_r, _g, _b);
}

void Material::setSpecularColor(GLfloat _r, GLfloat _g, GLfloat _b, float _shininess)
{
	spec_color = glm::vec3(_r, _g, _b);
	shininess = _shininess;
}

void Material::sendShaderData(GLuint _program)
{
	glUniform1i(glGetUniformLocation(_program, "material.use_diff_map"), use_diff_map);
	glUniform1i(glGetUniformLocation(_program, "material.use_spec_map"), use_spec_map);
	glUniform1f(glGetUniformLocation(_program, "material.shininess"), shininess);
	glUniform3f(glGetUniformLocation(_program, "material.diff_color"), diff_color.r, diff_color.g, diff_color.b);
	glUniform3f(glGetUniformLocation(_program, "material.spec_color"), spec_color.r, spec_color.g, spec_color.b);
	diffuse.bind(_program, 0, "material.diffuse");
	specular.bind(_program, 1, "material.specular");
}


