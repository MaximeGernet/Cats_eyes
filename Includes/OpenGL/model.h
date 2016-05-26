/*
 * model.h
 *
 *  Created on: Apr 16, 2016
 *      Author: root
 */


/***************************************** Présentation ***********************************************
 *
 * But : charger un modèle 3D à partir d'un fichier quelconque
 *
 * Fonctionne sur des modèles simples
 *
 * La difficulté réside dans le chargement des matériaux
 * Pour le moment, il est possible de charger une diffuse map et une specular map, et de charger
 * une diffuse color et une specular color dans le cas où l'une des maps n'est pas utilisée.
 * La shininess est également chargée.
 *
 ******************************************************************************************************/

#ifndef MODEL_H_
#define MODEL_H_

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "vertex.h"
#include "material.h"

using namespace std;


class Mesh {
public:
	/* Mesh Data */
	std::vector<Vertex_s> vertices;	// position / normal / texCoord
	std::vector<GLuint> indices;
	Material material;
	//std::vector<Texture_s> textures;
	/* Functions */
	Mesh(std::vector<Vertex_s> _vertices, std::vector<GLuint> _indices, Material _material);
	void Draw(GLuint _program);
private:
	/* Render data */
	GLuint VAO, VBO, EBO;
	/* Functions
	*/
	void setupMesh();
};

class Model
{
public:
	std::vector<Texture> textures_loaded;
	/* Functions */
	Model(GLchar* path)
	{
		this->loadModel(path);
	}
	void Draw(GLuint _program);
private:
	/* Model Data */
	std::vector<Mesh> meshes;
	std::string directory;
	/* Functions */
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	Texture loadMaterialTexture(aiMaterial* mat, aiTextureType type);
};

#endif /* MODEL_H_ */
