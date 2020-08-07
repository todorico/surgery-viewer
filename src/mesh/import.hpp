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

// Renvoie une structure de scene assimp à partir d'un fichier contenant une description d'objets géométriques.
std::unique_ptr<aiScene> import_scene(const std::string& filename);
void print_scene_status(const aiScene* scene);

// Renvoie L'identifiant du premier maillage trouvé dans la scène sinon renvoie std::numeric_limits<unsigned int>::max().
unsigned int find_mesh_index(const aiScene* scene);
unsigned int find_mesh_index(const aiScene* scene, const aiNode* node);

// Renvoie le premier nom de texture trouvé dans la scène sinon renvoie une chaine vide.
std::string find_texture_name(const aiMaterial* material);
std::string find_texture_path(const std::string& filename,
							  const aiMaterial* material);

// Construie une Surface_mesh à partir d'une aiMesh contenu dans une scene aiScene.
Surface_mesh make_surface_mesh(const aiMesh* mesh_data);

#endif // MESH_IMPORT_HPP