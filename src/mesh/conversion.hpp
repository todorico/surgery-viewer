#ifndef MESH_CONVERT_HPP
#define MESH_CONVERT_HPP

// PROJECT

#include "data.hpp"

// STD

#include <optional>

// Convert Mesh_data to Surface_mesh

template <class SurfaceMesh>
SurfaceMesh to_surface_mesh(const Mesh_data& mesh_data);

// Convert Surface_mesh to Mesh_data

template <class SurfaceMesh>
Mesh_data to_mesh_data(const SurfaceMesh& mesh, std::optional<std::string> texture_path);

template <class SurfaceMesh>
Mesh_data to_mesh_data(const SurfaceMesh& mesh);

#include "conversion.inl"

#endif // MESH_CONVERT_HPP