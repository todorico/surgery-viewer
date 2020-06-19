#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

// STD

#include <iostream>
#include <optional>
#include <string>
#include <vector>

// ASSIMP

#include <assimp/scene.h>

// STD

#include <array>
#include <optional>

struct Mesh_data
{
	using vec_2f = std::array<float, 2>;
	using vec_3f = std::array<float, 3>;
	using vec_4f = std::array<float, 4>;
	using vec_3u = std::array<unsigned int, 3>;

	std::optional<std::vector<vec_3f>> positions;
	std::optional<std::vector<vec_3f>> normals;
	std::optional<std::vector<vec_4f>> colors;
	std::optional<std::vector<vec_2f>> texcoords;
	std::optional<std::vector<vec_3u>> triangulated_faces; // index of 3 connected vertex

	std::optional<std::string> texture_path;
};

Mesh_data load_mesh_data(const std::string& filename);

std::optional<Mesh_data> find_first_mesh_data(aiNode* node, const aiScene* scene);

// Does not return full path of texture file but only the filename
std::optional<std::string> find_first_material_texture_name(aiMaterial* mat);

#include "data.inl"

#endif // MESH_DATA_HPP