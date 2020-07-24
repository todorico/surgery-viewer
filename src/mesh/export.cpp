#include "export.hpp"

// STD

#include <iostream>
#include <string>

// ASSIMP

#include <assimp/Exporter.hpp>

std::unique_ptr<aiMesh> make_ai_mesh(const Surface_mesh& surface_mesh)
{
	using Vertex_index = Surface_mesh::Vertex_index;
	using size_type	   = Surface_mesh::size_type;

	using Vector_3 = Kernel::Vector_3;
	using Vector_2 = Kernel::Vector_2;

	aiMesh* mesh_data = new aiMesh();

	std::map<Vertex_index, unsigned int> indice_map;

	mesh_data->mNumVertices = surface_mesh.number_of_vertices();

	// point map exist by default for surface_mesh
	{
		size_type i = 0;

		mesh_data->mVertices = new aiVector3D[mesh_data->mNumVertices];

		for(auto v : surface_mesh.vertices())
		{
			auto position = surface_mesh.point(v);

			mesh_data->mVertices[i] = {static_cast<float>(position[0]),
									   static_cast<float>(position[1]),
									   static_cast<float>(position[2])};
			indice_map[v]			= i;
			++i;
		}
	}

	auto [normal_map, normal_map_exist] =
		surface_mesh.template property_map<Vertex_index, Vector_3>("v:normal");

	if(normal_map_exist)
	{
		size_type i = 0;

		mesh_data->mNormals = new aiVector3D[mesh_data->mNumVertices];

		for(auto v : surface_mesh.vertices())
		{
			auto normal			   = normal_map[v];
			mesh_data->mNormals[i] = {static_cast<float>(normal[0]),
									  static_cast<float>(normal[1]),
									  static_cast<float>(normal[2])};
			++i;
		}
	}
	else
	{
		std::clog << "[STATUS] to_mesh_data : no normal map found\n";
	}

	auto [color_map, color_map_exist] =
		surface_mesh.template property_map<Vertex_index, std::array<float, 4>>(
			"v:color");

	if(color_map_exist)
	{
		size_type i = 0;

		mesh_data->mColors[0] = new aiColor4D[mesh_data->mNumVertices];

		for(auto v : surface_mesh.vertices())
		{
			auto color				 = color_map[v];
			mesh_data->mColors[0][i] = {color[0], color[1], color[2], color[3]};
			++i;
		}
	}
	else
	{
		std::clog << "[STATUS] to_mesh_data : no color map found\n";
	}

	auto [texcoord_map, texcoord_map_exist] =
		surface_mesh.template property_map<Vertex_index, Vector_2>(
			"v:texcoord");

	if(texcoord_map_exist)
	{
		size_type i = 0;

		mesh_data->mTextureCoords[0] = new aiVector3D[mesh_data->mNumVertices];

		for(auto v : surface_mesh.vertices())
		{
			auto texcoord					= texcoord_map[v];
			mesh_data->mTextureCoords[0][i] = {static_cast<float>(texcoord[0]),
											   static_cast<float>(texcoord[1]),
											   0.0f};
			++i;
		}
	}
	else
	{
		std::clog << "[STATUS] to_mesh_data : no texcoord map found\n";
	}

	mesh_data->mNumFaces = surface_mesh.number_of_faces();

	// face map exist by default on surface_mesh
	{
		size_type f = 0;

		mesh_data->mFaces = new aiFace[mesh_data->mNumFaces];

		for(auto face : surface_mesh.faces())
		{
			auto face_vertices = CGAL::vertices_around_face(
				surface_mesh.halfedge(face), surface_mesh);

			mesh_data->mFaces[f].mNumIndices =
				static_cast<unsigned int>(face_vertices.size());
			mesh_data->mFaces[f].mIndices =
				new unsigned int[mesh_data->mFaces[f].mNumIndices];

			size_type i = 0;

			for(auto v : face_vertices)
			{
				mesh_data->mFaces[f].mIndices[i] = indice_map[v];
				++i;
			}

			++f;
		}
	}

	return std::unique_ptr<aiMesh>(mesh_data);
}

void assign_scene_mesh(aiScene* scene, unsigned int scene_mesh_index,
					   aiMesh* new_mesh)
{
	delete scene->mMeshes[scene_mesh_index];
	scene->mMeshes[scene_mesh_index] = new_mesh;
}

aiReturn export_scene(const std::string& format, const std::string& filename,
					  const aiScene* scene)
{
	Assimp::Exporter exporter;

	auto res = exporter.Export(scene, format, filename);

	if(res != aiReturn_SUCCESS)
	{
		std::cerr << "[ERROR] ASSIMP : " << exporter.GetErrorString() << '\n';
		exit(EXIT_FAILURE);
	}

	return res;
}
