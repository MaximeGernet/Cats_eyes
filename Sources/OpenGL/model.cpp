/*
 * model.cpp
 *
 *  Created on: Apr 16, 2016
 *      Author: root
 */

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL/SOIL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <iostream>

#include "vertex.h"
#include "material.h"
#include "model.h"

Mesh::Mesh(std::vector<Vertex_s> _vertices, std::vector<GLuint> _indices, Material _material)
{
	this->vertices = _vertices;
	this->indices = _indices;
	this->material = _material;
	this->setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex_s), &this->vertices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);
			// Vertex Positions
			glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_s),	(GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_s),	(GLvoid*)offsetof(Vertex_s, Normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_s),	(GLvoid*)offsetof(Vertex_s, TexCoords));
	glBindVertexArray(0);
}

void Mesh::Draw(GLuint _program)
{
	material.sendShaderData(_program);
	/*
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for(GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
		// Retrieve texture number (the N in diffuse_textureN)
		std::stringstream ss;
		std::string number;
		std::string name = this->textures[i].type;
		if(name == "texture_diffuse")
			ss << diffuseNr++; // Transfer GLuint to stream
		else if(name == "texture_specular")
			ss << specularNr++; // Transfer GLuint to stream
		number = ss.str();
		glUniform1f(glGetUniformLocation(_program, ("material." + name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}
	*/
	//glActiveTexture(GL_TEXTURE0);
	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


void Model::Draw(GLuint _program)
{
	for(GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].Draw(_program);
}

void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	//path.substr(0, path.find_last_of('/'));
	this->directory = "";
	//std::cout << directory << std::endl;
	this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node’s meshes (if any)
	for(GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	// Then do the same for each of its children
	for(GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex_s> vertices;
	std::vector<GLuint> indices;
	Material material;
	Texture diffuse, specular;

	for(GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex_s vertex;
		glm::vec3 vector;
		// Process vertex positions, normals and texture coordinates
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;


		if(mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// Process indices
	for(GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for(GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// Process material
	printf("material index: %d\n", mesh->mMaterialIndex);
	if(mesh->mMaterialIndex >= 0)
	{
		aiMaterial* aimaterial = scene->mMaterials[mesh->mMaterialIndex];
		//std::cout << "Number of properties: " << aimaterial->mNumProperties << std::endl;
		for(int l = 0; l < aimaterial->mNumProperties; l++)
		{
			std::string property = std::string(aimaterial->mProperties[l]->mKey.C_Str());
			//std::cout << property << std::endl;
		}

		// 0. Shininess
		float shininess = 0.0f;
		aiGetMaterialFloat(aimaterial, AI_MATKEY_SHININESS, &shininess);
		if(shininess < 1.0f)
			shininess = 1.0f;
		// 1. Diffuse map
		if(aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			diffuse = loadMaterialTexture(aimaterial, aiTextureType_DIFFUSE);
			material.setDiffuseMap(diffuse);
		}
		// 2. Specular maps
		if(aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			specular = loadMaterialTexture(aimaterial, aiTextureType_SPECULAR);
			material.setSpecularMap(specular, shininess);
		}

		//std::cout << "Shininess: " << shininess << std::endl;
		aiColor4D aicolor;
		aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_DIFFUSE, &aicolor);
		material.setDiffuseColor(aicolor.r, aicolor.g, aicolor.b);
		//std::cout << "Diffuse: " << aicolor.r << ", " << aicolor.g  << ", " << aicolor.b << std::endl;
		aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_SPECULAR, &aicolor);
		material.setSpecularColor(aicolor.r, aicolor.g, aicolor.b, shininess);
		//std::cout << "Specular: " << aicolor.r << ", " << aicolor.g  << ", " << aicolor.b << std::endl;


		/*
		for(int l = 0; l < aimaterial->mNumProperties; l++)
		{
			if(strcmp("$mat.shininess", aimaterial->mProperties[l]->mKey.C_Str()) == 0)
				aiGetMaterialProperty(aimaterial, AI_MATKEY_SHININESS
		}
		*/
	}

	return Mesh(vertices, indices, material);
}

Texture Model::loadMaterialTexture(aiMaterial* mat, aiTextureType type)
{
	Texture texture;
	aiString str;
	mat->GetTexture(type, 0, &str);
	GLboolean skip = false;

	for(GLuint j = 0; j < textures_loaded.size(); j++)
	{
		if(textures_loaded[j].path() == str)
		{
			texture = textures_loaded[j];
			skip = true;
			break;
		}
	}
	if(!skip)
	{ // If texture hasn’t been loaded already, load it
		std::string filename_s = std::string(str.C_Str());
		char* str_c = (char*)filename_s.c_str();
		printf("%s\n", str_c);
		int c = 0;
		printf("%d\n", c);
		while(c < filename_s.size() && str_c[c] != 'H' && str_c[c] != 'S')
			c++;
		printf("%d\n", c);
		std::string file_path = std::string(&str_c[c]);
		file_path.substr(0, file_path.find_last_of(92));
		//file_path = "Textures/" + file_path;
		//file_path = "P-51 Mustang/" + file_path;
		file_path = "Array_House_Example/textures/" + file_path;
		std::cout << file_path << std::endl;
		texture.load(file_path.c_str());
		texture.setPath(str);
		this->textures_loaded.push_back(texture); // Add to loaded textures
	}

	return texture;
}
