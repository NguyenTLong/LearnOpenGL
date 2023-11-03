#pragma once
#ifndef MODEL_H
#define MODEL_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
using namespace glm;


class Model
{
public:
	void Init(const string &path);
	void Draw(Shader& shader);
private:
	vector<Texture> textures_loaded;

	vector<Mesh>meshes;
	string directory;

	void loadModel(const string &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	bool LoadCachedTexture(aiString str, vector<Texture> &textures);
};
#endif
