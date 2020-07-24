#include "import.hpp"

// STD

#include <iostream>
#include <string>

// ASSIMP

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

void print_scene_status(const aiScene* scene)
{
	if(scene->HasAnimations())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumAnimations
				  << " animation(s)\n";

	if(scene->HasCameras())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumCameras
				  << " camera(s)\n";

	if(scene->HasLights())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumLights
				  << " light(s)\n";

	if(scene->HasMaterials())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumMaterials
				  << " material(s)\n";

	if(scene->HasMeshes())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumMeshes
				  << " mesh(es)\n";

	if(scene->HasTextures())
		std::clog << "[STATUS] ASSIMP SCENE has " << scene->mNumTextures
				  << " texture(s)\n";
}

unsigned int find_mesh_index(const aiScene* scene)
{
	return find_mesh_index(scene, scene->mRootNode);
}

unsigned int find_mesh_index(const aiScene* scene, const aiNode* node)
{
	// A mesh is adequate if she has at least positions, normals and faces.

	if(!node)
	{
		return std::numeric_limits<unsigned int>::max();
	}
	else
	{
		auto meshes_index_begin = node->mMeshes;
		auto meshes_index_end	= node->mMeshes + node->mNumMeshes;

		auto index_it = std::find_if(meshes_index_begin, meshes_index_end,
									 [&scene](const unsigned int index) {
										 aiMesh* mesh = scene->mMeshes[index];
										 return mesh->HasPositions() &&
												mesh->HasNormals() &&
												mesh->HasFaces();
									 });

		if(index_it != meshes_index_end)
		{
			return *index_it;
		}
		else
		{
			for(unsigned int i = 0; i < node->mNumChildren; i++)
			{
				return find_mesh_index(scene, node->mChildren[i]);
			}

			return std::numeric_limits<unsigned int>::max();
		}
	}
}

std::unique_ptr<aiScene> import_scene(const std::string& filename)
{
	// [WARNING] scene data should not be read after importer is
	// destroyed because he is in charge of memory allocation.
	Assimp::Importer importer;

	// Specify both aiProcess_Triangulate and aiProcess_SortByPType
	// Triangulate mesh and Ignore all point and line meshes when you
	// process assimp's output
	importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenNormals |
									aiProcess_SortByPType);

	aiScene* scene = importer.GetOrphanedScene();

	if(!scene)
	{
		std::cerr << "[ERROR] ASSIMP : " << importer.GetErrorString() << '\n';
		exit(EXIT_FAILURE);
	}

	return std::unique_ptr<aiScene>(scene);
}

std::string find_texture_name(const aiMaterial* material)
{
	for(unsigned int type = 0; type < AI_TEXTURE_TYPE_MAX; ++type)
	{
		aiTextureType t = static_cast<aiTextureType>(type);

		for(unsigned int i = 0; i < material->GetTextureCount(t); i++)
		{
			aiString str;
			material->GetTexture(t, i, &str);

			std::clog << "[STATUS] ASSIMP MATERIAL : texture of type " << t
					  << " found (" << str.C_Str() << ")\n";

			return str.C_Str();
		}
	}

	std::clog << "[STATUS] ASSIMP MATERIAL : no texture found\n";
	return "";
}

std::string find_texture_path(const std::string& filename,
							  const aiMaterial* material)
{
	std::string texture_path;

	std::string texture_name = find_texture_name(material);

	if(!texture_name.empty())
	{
		std::string texture_directory =
			filename.substr(0, filename.find_last_of("/\\"));

		// Add full path to texture name

		texture_path = texture_directory + '/' + texture_name;
	}

	return texture_path;
}

Surface_mesh make_surface_mesh(const aiMesh* mesh_data)
{
	using Vertex_index = Surface_mesh::Vertex_index;
	using size_type	   = Surface_mesh::size_type;

	using Vector_3 = Kernel::Vector_3;
	using Vector_2 = Kernel::Vector_2;

	Surface_mesh surface_mesh;

	if(mesh_data->HasPositions())
	{
		for(unsigned int i = 0; i < mesh_data->mNumVertices; ++i)
		{
			aiVector3D position = mesh_data->mVertices[i];
			surface_mesh.add_vertex({position[0], position[1], position[2]});
		}
	}
	else
	{
		std::clog << "[STATUS] surface_mesh : contains no vertex positions\n";
	}

	if(mesh_data->HasNormals())
	{
		auto [normal_map, normal_map_created] =
			surface_mesh.template add_property_map<Vertex_index, Vector_3>(
				"v:normal");

		for(unsigned int i = 0; i < mesh_data->mNumVertices; ++i)
		{
			aiVector3D normal = mesh_data->mNormals[i];
			Vertex_index v	  = static_cast<Vertex_index>(i);
			normal_map[v]	  = {normal[0], normal[1], normal[2]};
		}
	}
	else
	{
		std::clog << "[STATUS] surface_mesh : contains no vertex normals\n";
	}

	// Cherche uniquement dans le color set 0

	if(mesh_data->HasVertexColors(0))
	{
		auto [color_map, color_map_created] =
			surface_mesh
				.template add_property_map<Vertex_index, std::array<float, 4>>(
					"v:color");

		for(unsigned int i = 0; i < mesh_data->mNumVertices; ++i)
		{
			aiColor4D color = mesh_data->mColors[0][i];
			Vertex_index v	= static_cast<Vertex_index>(i);
			color_map[v]	= {color[0], color[1], color[2], color[3]};
		}
	}
	else
	{
		std::clog << "[STATUS] surface_mesh : contains no vertex colors\n";
	}

	// Cherche uniquement dans le texture set 0

	if(mesh_data->HasTextureCoords(0))
	{
		auto [texcoord_map, texcoord_map_created] =
			surface_mesh.template add_property_map<Vertex_index, Vector_2>(
				"v:texcoord");

		for(unsigned int i = 0; i < mesh_data->mNumVertices; ++i)
		{
			aiVector3D texcoord = mesh_data->mTextureCoords[0][i];
			Vertex_index v		= static_cast<Vertex_index>(i);
			texcoord_map[v]		= {texcoord[0], texcoord[1]};
		}
	}
	else
	{
		std::clog << "[STATUS] surface_mesh : contains no vertex texcoords\n";
	}

	if(mesh_data->HasFaces())
	{
		for(unsigned int i = 0; i < mesh_data->mNumFaces; ++i)
		{
			aiFace face = mesh_data->mFaces[i];

			if(face.mNumIndices != 3)
			{
				std::clog << "[STATUS] surface_mesh : skipped face " << i
						  << " that is not a triangle\n";
				continue;
			}

			Vertex_index a = static_cast<Vertex_index>(face.mIndices[0]);
			Vertex_index b = static_cast<Vertex_index>(face.mIndices[1]);
			Vertex_index c = static_cast<Vertex_index>(face.mIndices[2]);

			surface_mesh.add_face(a, b, c);
		}
	}

	return surface_mesh;
}