#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

// STD

#include <iostream>
#include <optional>
#include <string>
#include <vector>

// GLM

#include <glm/glm.hpp>

// ASSIMP

#include <assimp/scene.h>

struct Mesh_data
{
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::optional<std::vector<glm::vec2>> uvs;
	std::optional<std::string> texture_path;
};

Mesh_data load_mesh_data(const std::string& filename);

std::optional<Mesh_data> find_first_mesh_data(aiNode* node, const aiScene* scene);

// Does not return full path of texture file but only the name
std::optional<std::string> find_first_material_texture_name(aiMaterial* mat);

#include "data.inl"

#endif // MESH_DATA_HPP