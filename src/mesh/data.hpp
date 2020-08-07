#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

// STD

#include <array>
#include <optional>
#include <string>
#include <vector>

// Cette structure est utilisée par le viewer pour afficher les maillages
// Il est possible de convertir Surface_mesh en Mesh_data grâce à 'to_mesh_data'
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

#endif // MESH_DATA_HPP