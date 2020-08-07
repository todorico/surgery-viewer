#ifndef MESH_CONVERT_HPP
#define MESH_CONVERT_HPP

// PROJECT

#include "../instance/Surface_mesh.hpp"
#include "data.hpp"

// STD

#include <optional>

// Convert Mesh_data to Surface_mesh
Surface_mesh to_surface_mesh(const Mesh_data& mesh_data);

// Convert Surface_mesh to Mesh_data
Mesh_data to_mesh_data(const Surface_mesh& mesh,
					   const std::string& texture_path);

Mesh_data to_mesh_data(const Surface_mesh& mesh);

// #include "conversion.inl"

#endif // MESH_CONVERT_HPP