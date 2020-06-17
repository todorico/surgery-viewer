#ifndef MESH_CONVERT_HPP
#define MESH_CONVERT_HPP

// PROJECT

#include "data.hpp"

// CGAL

#include <CGAL/IO/Color.h>
#include <CGAL/Kernel_traits.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/iterator.h>

// STD

#include <array>

template <class SurfaceMesh>
SurfaceMesh to_surface_mesh(const Mesh_data& mesh_data)
{
	// using SurfaceMesh = typename CGAL::Surface_mesh<Point>;
	using point_type = typename SurfaceMesh::Point;
	using size_type	 = typename SurfaceMesh::size_type;

	using Kernel = typename CGAL::Kernel_traits<point_type>::Kernel;

	using Vertex_index = typename SurfaceMesh::Vertex_index;
	using Vector_3	   = typename Kernel::Vector_3;
	using Vector_2	   = typename Kernel::Vector_2;
	using Color		   = CGAL::Color;

	SurfaceMesh mesh;

	if(mesh_data.positions.has_value())
	{
		for(size_t i = 0; i < mesh_data.positions->size(); ++i)
		{
			auto position = (*mesh_data.positions)[i];
			mesh.add_vertex({position[0], position[1], position[2]});
		}
	}

	if(mesh_data.normals.has_value())
	{
		auto [normal_map, normal_map_created] =
			mesh.template add_property_map<Vertex_index, Vector_3>("v:normal");

		for(size_type i = 0; i < mesh_data.normals->size(); ++i)
		{
			auto normal	   = (*mesh_data.normals)[i];
			Vertex_index v = static_cast<Vertex_index>(i);
			normal_map[v]  = {normal[0], normal[1], normal[2]};
		}
	}
	else
	{
		std::clog << "[STATUS] to_surface_mesh : data contains no vertex normals\n";
	}

	if(mesh_data.colors.has_value())
	{
		auto [color_map, color_map_created] =
			mesh.template add_property_map<Vertex_index, Color>("v:color");

		for(size_type i = 0; i < mesh_data.colors->size(); ++i)
		{
			auto color	   = (*mesh_data.colors)[i];
			Vertex_index v = static_cast<Vertex_index>(i);
			color_map[v]   = {color[0] * 255, color[1] * 255, color[2] * 255, color[3] * 255};
		}
	}
	else
	{
		std::clog << "[STATUS] to_surface_mesh : data contains no vertex colors\n";
	}

	if(mesh_data.texcoords.has_value())
	{
		auto [texcoord_map, texcoord_map_created] =
			mesh.template add_property_map<Vertex_index, Vector_2>("v:texcoord");

		for(size_type i = 0; i < mesh_data.texcoords->size(); ++i)
		{
			auto texcoord	= (*mesh_data.texcoords)[i];
			Vertex_index v	= static_cast<Vertex_index>(i);
			texcoord_map[v] = Vector_2(texcoord[0], texcoord[1]);
		}
	}
	else
	{
		std::clog << "[STATUS] to_surface_mesh : data contains no texcoords\n";
	}

	if(mesh_data.triangulated_faces.has_value())
	{
		for(size_type i = 0; i < mesh_data.triangulated_faces->size(); ++i)
		{
			Vertex_index a = static_cast<Vertex_index>((*mesh_data.triangulated_faces)[i][0]);
			Vertex_index b = static_cast<Vertex_index>((*mesh_data.triangulated_faces)[i][1]);
			Vertex_index c = static_cast<Vertex_index>((*mesh_data.triangulated_faces)[i][2]);

			mesh.add_face(a, b, c);
		}
	}
	else
	{
		std::clog << "[STATUS] to_surface_mesh : data contains no faces\n";
	}

	return mesh;
}

template <class SurfaceMesh>
Mesh_data to_mesh_data(const SurfaceMesh& mesh, std::optional<std::string> texture_path)
{
	using point_type = typename SurfaceMesh::Point;
	using size_type	 = typename SurfaceMesh::size_type;

	using Kernel	   = typename CGAL::Kernel_traits<point_type>::Kernel;
	using Vertex_index = typename SurfaceMesh::Vertex_index;
	using Vector_3	   = typename Kernel::Vector_3;
	using Vector_2	   = typename Kernel::Vector_2;
	using Color		   = CGAL::Color;

	if(mesh.number_of_vertices() == 0)
	{
		std::cerr << "[WARNING] to_mesh_data : contains no vertices\n";
		return Mesh_data{};
	}

	std::map<Vertex_index, unsigned int> indice_map;

	std::optional<std::vector<Mesh_data::vec_3f>> positions;

	// point map exist by default on surface_mesh
	{
		size_type i = 0;

		positions.emplace(std::vector<Mesh_data::vec_3f>(mesh.number_of_vertices()));

		for(auto v : mesh.vertices())
		{
			auto position	= mesh.point(v);
			(*positions)[i] = {static_cast<float>(position[0]), static_cast<float>(position[1]),
							   static_cast<float>(position[2])};
			indice_map[v]	= i;
			++i;
			// 	std::cerr << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z <<
			// '\n';
		}
	}

	std::optional<std::vector<Mesh_data::vec_3f>> normals;

	auto [normal_map, normal_map_exist] =
		mesh.template property_map<Vertex_index, Vector_3>("v:normal");

	if(normal_map_exist)
	{
		size_type i = 0;

		normals.emplace(std::vector<Mesh_data::vec_3f>(mesh.number_of_vertices()));

		for(auto v : mesh.vertices())
		{
			auto normal	  = normal_map[v];
			(*normals)[i] = {static_cast<float>(normal[0]), static_cast<float>(normal[1]),
							 static_cast<float>(normal[2])};
			++i;
		}
	}
	else
	{
		std::clog << "[STATUS] to_mesh_data : no normal map found\n";
	}

	std::optional<std::vector<Mesh_data::vec_4f>> colors;

	auto [color_map, color_map_exist] = mesh.template property_map<Vertex_index, Color>("v:color");

	if(color_map_exist)
	{
		size_type i = 0;

		colors.emplace(std::vector<Mesh_data::vec_4f>(mesh.number_of_vertices()));

		for(auto v : mesh.vertices())
		{
			auto color	 = color_map[v];
			(*colors)[i] = {color[0] / 255.0, color[1] / 255.0, color[2] / 255.0, color[3] / 255.0};
			++i;
		}
	}
	else
	{
		std::clog << "[STATUS] to_mesh_data : no color map found\n";
	}

	std::optional<std::vector<Mesh_data::vec_2f>> texcoords;

	auto [texcoord_map, texcoord_map_exist] =
		mesh.template property_map<Vertex_index, Vector_2>("v:texcoord");

	if(texcoord_map_exist)
	{
		size_type i = 0;

		texcoords.emplace(std::vector<Mesh_data::vec_2f>(mesh.number_of_vertices()));

		for(auto v : mesh.vertices())
		{
			auto texcoord	= texcoord_map[v];
			(*texcoords)[i] = {static_cast<float>(texcoord[0]), static_cast<float>(texcoord[1])};
			++i;
		}
	}
	else
	{
		std::clog << "[STATUS] to_mesh_data : no texcoord map found\n";
	}

	std::optional<std::vector<Mesh_data::vec_3u>> triangulated_faces;

	// face map exist by default on surface_mesh
	{
		size_type f = 0;

		triangulated_faces.emplace(std::vector<Mesh_data::vec_3u>(mesh.number_of_faces()));

		for(auto face : mesh.faces())
		{
			auto face_vertices = CGAL::vertices_around_face(mesh.halfedge(face), mesh);

			if(face_vertices.size() != 3)
			{
				std::cerr << "[WARNING] to_mesh_data : skipped face " << face
						  << " that is not a triangle\n";
				continue;
			}

			size_type i = 0;

			for(auto v : face_vertices)
			{
				(*triangulated_faces)[f][i] = indice_map[v];
				++i;
			}

			++f;
		}
	}

	return Mesh_data{positions, normals, colors, texcoords, triangulated_faces, texture_path};
}

template <class SurfaceMesh>
Mesh_data to_mesh_data(const SurfaceMesh& mesh)
{
	return to_mesh_data<SurfaceMesh>(mesh, {});
}

// #include "conversion.inl"

#endif // MESH_CONVERT_HPP