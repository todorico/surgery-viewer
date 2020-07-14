// #ifndef MESH_DATA_INL
// #define MESH_DATA_INL

#include "data.hpp"

// STD

#include <iostream>
#include <string>

// ASSIMP

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Mesh_data load_mesh_data(const std::string& filename)
{
	Assimp::Importer importer;

	// Specify both aiProcess_Triangulate and aiProcess_SortByPType
	// Triangulate mesh and Ignore all point and line meshes when you process assimp's output

	const aiScene* scene = importer.ReadFile(
		filename, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_SortByPType);

	if(!scene)
	{
		std::cerr << "[ERROR] ASSIMP : " << importer.GetErrorString() << '\n';
		exit(EXIT_FAILURE);
	}

	if(scene->HasAnimations())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumAnimations << " animation(s)\n";

	if(scene->HasCameras())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumCameras << " camera(s)\n";

	if(scene->HasLights())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumLights << " light(s)\n";

	if(scene->HasMaterials())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumMaterials << " material(s)\n";

	if(scene->HasMeshes())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumMeshes << " mesh(es)\n";

	if(scene->HasTextures())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumTextures << " texture(s)\n";

	std::optional<Mesh_data> data = find_first_mesh_data(scene->mRootNode, scene);

	if(!data)
	{
		std::cerr << "[ERROR] ASSIMP : no suitable mesh found\n";
		exit(EXIT_FAILURE);
	}

	// Currently if texture_path exist, it contains only a name and not a full path

	if(data->texture_path.has_value())
	{
		std::string texture_name	  = data->texture_path.value();
		std::string texture_directory = filename.substr(0, filename.find_last_of("/\\"));

		// Add full path to texture name

		data->texture_path = texture_directory + '/' + texture_name;
	}

	return data.value();
}

std::optional<Mesh_data> find_first_mesh_data(aiNode* node, const aiScene* scene)
{
	// Process each mesh located at the current node

	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		if(!(mesh->HasPositions() && mesh->HasNormals() && mesh->HasFaces()))
		{
			std::clog
				<< "[WARNING] ASSIMP MESH : skipped mesh without positions, normals or faces \n";
			continue;
		}

		std::optional<std::vector<Mesh_data::vec_3f>> positions;

		if(mesh->HasPositions())
		{
			positions.emplace(std::vector<Mesh_data::vec_3f>(mesh->mNumVertices));

			for(unsigned int k = 0; k < mesh->mNumVertices; ++k)
			{
				aiVector3D position = mesh->mVertices[k];
				(*positions)[k]		= {position.x, position.y, position.z};
			}
		}

		std::optional<std::vector<Mesh_data::vec_3f>> normals;

		if(mesh->HasNormals())
		{
			normals.emplace(std::vector<Mesh_data::vec_3f>(mesh->mNumVertices));

			for(unsigned int k = 0; k < mesh->mNumVertices; ++k)
			{
				aiVector3D normal = mesh->mNormals[k];
				(*normals)[k]	  = {normal.x, normal.y, normal.z};
			}
		}

		std::optional<std::vector<Mesh_data::vec_4f>> colors;

		// Cherche uniquement dans le color_set 0

		if(mesh->HasVertexColors(0))
		{
			colors.emplace(std::vector<Mesh_data::vec_4f>(mesh->mNumVertices));

			for(unsigned int k = 0; k < mesh->mNumVertices; ++k)
			{
				aiColor4D color = mesh->mColors[0][k];
				(*colors)[k]	= {color.r, color.g, color.b, color.a};
			}
		}

		std::optional<std::vector<Mesh_data::vec_2f>> texcoords;
		std::optional<std::string> texture_name;

		// Cherche uniquement dans le texture_set 0

		if(mesh->HasTextureCoords(0))
		{
			texture_name =
				find_first_material_texture_name(scene->mMaterials[mesh->mMaterialIndex]);

			if(!texture_name)
			{
				std::clog << "[WARNING] ASSIMP MESH : has texture coords but no associate "
							 "texture path has been found\n";
			}

			texcoords.emplace(std::vector<Mesh_data::vec_2f>(mesh->mNumVertices));

			for(unsigned int k = 0; k < mesh->mNumVertices; ++k)
			{
				aiVector3D texcoord = mesh->mTextureCoords[0][k];
				(*texcoords)[k]		= {texcoord.x, texcoord.y};
			}
		}

		std::optional<std::vector<Mesh_data::vec_3u>>
			triangulated_faces; // index of 3 connected vertex

		if(mesh->HasFaces())
		{
			triangulated_faces.emplace(std::vector<Mesh_data::vec_3u>(mesh->mNumFaces));

			for(unsigned int k = 0; k < mesh->mNumFaces; ++k)
			{
				aiFace face = mesh->mFaces[k];

				if(face.mNumIndices != 3)
				{
					std::clog << "[WARNING] ASSIMP MESH : skipped face " << k
							  << " that is not a triangle\n";
					continue;
				}

				(*triangulated_faces)[k] = {face.mIndices[0], face.mIndices[1], face.mIndices[2]};
			}
		}

		return Mesh_data{positions, normals, colors, texcoords, triangulated_faces, texture_name};
	}

	// Process children nodes if no mesh where found

	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		return find_first_mesh_data(node->mChildren[i], scene);
	}

	return {};
}

std::optional<std::string> find_first_material_texture_name(aiMaterial* mat)
{
	for(unsigned int type = 0; type < AI_TEXTURE_TYPE_MAX; ++type)
	{
		aiTextureType t = static_cast<aiTextureType>(type);

		for(unsigned int i = 0; i < mat->GetTextureCount(t); i++)
		{
			aiString str;
			mat->GetTexture(t, i, &str);

			std::clog << "[STATUS] ASSIMP MATERIAL : texture of type " << t << " found ("
					  << str.C_Str() << ")\n";

			return str.C_Str();
		}
	}

	std::clog << "[STATUS] ASSIMP MATERIAL : no texture found\n";

	return {};
}

// #endif // MESH_DATA_INL