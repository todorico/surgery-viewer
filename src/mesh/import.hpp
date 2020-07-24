#ifndef MESH_IMPORT_HPP
#define MESH_IMPORT_HPP

// PROJECT

#include "../instance/Surface_mesh.hpp"
#include "data.hpp"

// STD

#include <array>
#include <optional>
#include <string>
#include <vector>

// ASSIMP

#include <assimp/scene.h>

std::unique_ptr<aiScene> import_scene(const std::string& filename);
void print_scene_status(const aiScene* scene);

unsigned int find_mesh_index(const aiScene* scene);
unsigned int find_mesh_index(const aiScene* scene, const aiNode* node);

std::string find_texture_name(const aiMaterial* material);
std::string find_texture_path(const std::string& filename,
							  const aiMaterial* material);

Surface_mesh make_surface_mesh(const aiMesh* mesh_data);

#endif // MESH_IMPORT_HPP