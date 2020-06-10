#ifndef MESH_CONVERT_HPP
#define MESH_CONVERT_HPP

// PROJECT

#include "data.hpp"

// CGAL

#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/iterator.h>

template <class SurfaceMesh, class Kernel>
SurfaceMesh to_surface_mesh(const Mesh_data& mesh_data)
{
	SurfaceMesh mesh;

	using Vertex_index = typename SurfaceMesh::Vertex_index;
	using Point_3	   = typename Kernel::Point_3;
	using Vector_2	   = typename Kernel::Vector_2;
	using Vector_3	   = typename Kernel::Vector_3;

	// Copie des positions

	for(size_t i = 0; i < mesh_data.vertices.size(); ++i)
	{
		auto position = mesh_data.vertices[i];
		mesh.add_vertex(Point_3(position.x, position.y, position.z));
	}

	// Copie des normals

	auto [normal_map, normal_created] =
		mesh.template add_property_map<Vertex_index, Vector_3>("v:normal");

	for(size_t i = 0; i < mesh_data.normals.size(); ++i)
	{
		auto normal	   = mesh_data.normals[i];
		Vertex_index v = static_cast<Vertex_index>(i);
		normal_map[v]  = Vector_3(normal.x, normal.y, normal.z);
	}

	// Copie des coordonnées de textures si existantes

	if(mesh_data.texture_path.has_value())
	{
		auto [texcoord_map, texcoord_created] =
			mesh.template add_property_map<Vertex_index, Vector_2>("v:texcoord");

		for(size_t i = 0; i < mesh_data.uvs->size(); ++i)
		{
			auto texcoord	= (*mesh_data.uvs)[i];
			Vertex_index v	= static_cast<Vertex_index>(i);
			texcoord_map[v] = Vector_2(texcoord.x, texcoord.y);
		}
	}

	// Copie des faces triangulé

	for(size_t i = 2; i < mesh_data.indices.size(); i += 3)
	{
		Vertex_index a = static_cast<Vertex_index>(mesh_data.indices[i - 2]);
		Vertex_index b = static_cast<Vertex_index>(mesh_data.indices[i - 1]);
		Vertex_index c = static_cast<Vertex_index>(mesh_data.indices[i]);

		mesh.add_face(a, b, c);
	}

	return mesh;
}

template <class SurfaceMesh, class Kernel>
Mesh_data to_mesh_data(const SurfaceMesh& surface_mesh, std::optional<std::string> texture_path)
{
	using Vertex_index = typename SurfaceMesh::Vertex_index;
	// using Point_3	   = typename Kernel::Point_3;
	using Vector_2 = typename Kernel::Vector_2;
	using Vector_3 = typename Kernel::Vector_3;

	std::vector<glm::vec3> vertices(surface_mesh.number_of_vertices());
	std::map<Vertex_index, unsigned int> indice_map;

	size_t i = 0;

	std::cerr << "copieing vertices...\n";

	for(auto v : surface_mesh.vertices())
	{
		auto position = surface_mesh.point(v);
		vertices[i]	  = glm::vec3(position[0], position[1], position[2]);
		indice_map[v] = i;
		i++;
		// 	std::cerr << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << '\n';
	}

	std::vector<glm::vec3> normals(surface_mesh.number_of_vertices());

	auto [normal_map, normal_exist] =
		surface_mesh.template property_map<Vertex_index, Vector_3>("v:normal");

	if(normal_exist)
	{
		std::cerr << "[WARNING] SurfaceMesh conversion : no normal_map found\n";
	}

	i = 0;

	for(auto v : surface_mesh.vertices())
	{
		auto normal = normal_map[v];
		normals[i]	= glm::vec3(normal[0], normal[1], normal[2]);
		i++;
	}

	auto [texcoord_map, texcoord_exist] =
		surface_mesh.template property_map<Vertex_index, Vector_2>("v:texcoord");

	std::vector<glm::vec2> uvs(surface_mesh.number_of_vertices());

	if(texcoord_exist)
	{
		i = 0;

		for(auto v : surface_mesh.vertices())
		{
			auto uv = texcoord_map[v];
			uvs[i]	= glm::vec2(uv[0], uv[1]);
			i++;
		}
	}

	std::vector<unsigned int> indices(surface_mesh.number_of_faces() * 3);

	i = 0;

	for(auto face : surface_mesh.faces())
	{
		auto mesh_vertices = CGAL::vertices_around_face(surface_mesh.halfedge(face), surface_mesh);

		for(auto v : mesh_vertices)
		{
			indices[i] = indice_map[v]; // static_cast<unsigned int>(v);
			i++;
		}
	}

	return {indices, vertices, normals, uvs, texture_path};
}

template <class SurfaceMesh, class Kernel>
Mesh_data to_mesh_data(const SurfaceMesh& surface_mesh)
{
	return to_mesh_data<SurfaceMesh, Kernel>(surface_mesh, {});
}

// template <class Surface_mesh>
// Mesh_data to_mesh_data(const Surface_mesh& surface_mesh)
// {
// }

#include "conversion.inl"

#endif // MESH_CONVERT_HPP