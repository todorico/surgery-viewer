#ifndef MESH_DATA_INL
#define MESH_DATA_INL

#include "data.hpp"

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

	// Currently if texture_path exist, it contains only a name

	if(data->texture_path.has_value())
	{
		std::string texture_name	  = data->texture_path.value();
		std::string texture_directory = filename.substr(0, filename.find_last_of('/'));

		// Convert texture_path from name to full path

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

		// Mesh_data require at least positions normals and faces attributes

		if(!(mesh->HasPositions() && mesh->HasNormals() && mesh->HasFaces()))
		{
			std::clog << "[WARNING] ASSIMP MESH : skipped mesh without positions, normals or faces "
						 "attribute defined\n";
			continue;
		}

		// Loading positions, normals and faces

		std::vector<glm::vec3> vertices(mesh->mNumVertices);
		std::vector<glm::vec3> normals(mesh->mNumVertices);

		for(unsigned int k = 0; k < mesh->mNumVertices; ++k)
		{
			aiVector3D position = mesh->mVertices[k];
			aiVector3D normal	= mesh->mNormals[k];
			vertices[k]			= glm::vec3(position.x, position.y, position.z);
			normals[k]			= glm::vec3(normal.x, normal.y, normal.z);
		}

		std::vector<unsigned int> indices;

		for(unsigned int k = 0; k < mesh->mNumFaces; ++k)
		{
			aiFace face = mesh->mFaces[k];

			for(unsigned int j = 0; j < face.mNumIndices; ++j)
			{
				if(face.mNumIndices != 3)
				{
					std::clog << "[WARNING] ASSIMP MESH : this face is not a triangle\n";
				}

				indices.push_back(face.mIndices[j]);
			}
		}

		// Loading texture coords
		//
		// TODO : check if texture coord arity is 2 UV(x, y)

		if(mesh->HasTextureCoords(0))
		{
			auto texture_name =
				find_first_material_texture_name(scene->mMaterials[mesh->mMaterialIndex]);

			if(!texture_name)
			{
				std::clog << "[WARNING] ASSIMP MATERIAL : mesh has texture coords but no texture "
							 "where found\n";
			}
			else
			{
				std::vector<glm::vec2> uvs(mesh->mNumVertices);

				for(unsigned int k = 0; k < mesh->mNumVertices; ++k)
				{
					aiVector3D uv = mesh->mTextureCoords[0][k];
					uvs[k]		  = glm::vec2(uv.x, uv.y);
				}

				std::clog << "[STATUS] ASSIMP : found suitable mesh with texture\n";

				return Mesh_data{indices, vertices, normals, uvs, texture_name};
			}
		}

		std::clog << "[STATUS] ASSIMP : found suitable mesh without texture\n";
		return Mesh_data{indices, vertices, normals, {}, {}};
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

			std::clog << "[STATUS] ASSIMP MATERIAL texture of type " << t
					  << " found : " << str.C_Str() << '\n';

			return str.C_Str();
		}
	}

	std::clog << "[STATUS] ASSIMP MATERIAL no texture found\n";

	return {};
}

#endif // MESH_DATA_INL