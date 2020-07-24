#ifndef MESH_EXPORT_HPP
#define MESH_EXPORT_HPP

// PROJECT

#include "../instance/Surface_mesh.hpp"

// STD

#include <array>
#include <optional>
#include <string>
#include <vector>

// ASSIMP

#include <assimp/scene.h>

std::unique_ptr<aiMesh> make_ai_mesh(const Surface_mesh& surface_mesh);

void assign_scene_mesh(aiScene* scene, unsigned int scene_mesh_index,
					   aiMesh* new_mesh);

aiReturn export_scene(const std::string& format, const std::string& filename,
					  const aiScene* scene);

#endif // MESH_EXPORT_HPP