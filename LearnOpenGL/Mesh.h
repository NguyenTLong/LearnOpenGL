#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include "Shader.h"

using namespace std;
using namespace glm;

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
	unsigned int id;
	string type;
	string path;
};

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	void Draw(Shader shader);
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
private:
	unsigned int VAO, VBO, EBO;
	void SetupMesh();
};

#endif // !MESH_H
