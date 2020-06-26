#ifndef MESH_MARKING_INL
#define MESH_MARKING_INL

#include "marking.hpp"

// STD
#include <cmath>

// CGAL
#include <CGAL/boost/graph/iterator.h>

SM_marking_map get_marking_map(const Surface_mesh& mesh)
{
	auto [marking_map, marking_map_exist] =
		mesh.property_map<Surface_mesh::Vertex_index, Vertex_mark>("v:mark");

	assert(marking_map_exist);

	return marking_map;
}

SM_marking_map mark_regions(Surface_mesh& M1, const SM_kd_tree& M2_tree, double threshold)
{
	auto [marking_map, created] =
		M1.add_property_map<Surface_mesh::Vertex_index, Vertex_mark>("v:mark", Vertex_mark::None);

	for(auto v : M1.vertices())
	{
		SM_kd_tree_search search(M2_tree, M1.point(v), 1, 0, true,
								 M2_tree.traits().point_property_map());

		double distance = std::sqrt(search.begin()->second);

		if(distance > threshold)
		{
			marking_map[v] = Vertex_mark::Distant;
		}
		else
		{
			marking_map[v] = Vertex_mark::Close;
		}
	}

	return marking_map;
}

SM_marking_map mark_regions(Surface_mesh& M1, const Surface_mesh& M2, double threshold)
{
	SM_kd_tree M2_tree(M2.vertices().begin(), M2.vertices().end(), SM_kd_tree_splitter(),
					   SM_kd_tree_traits_adapter(M2.points()));
	return mark_regions(M1, M2_tree, threshold);
}

SM_marking_map mark_limits(const Surface_mesh& mesh)
{
	auto marking_map = get_marking_map(mesh);

	for(auto v : mesh.vertices())
	{
		if(marking_map[v] == Vertex_mark::Close)
		{
			auto around_vertices = CGAL::vertices_around_target(mesh.halfedge(v), mesh);

			for(auto i : around_vertices)
			{
				if(marking_map[i] == Vertex_mark::Distant)
				{
					marking_map[v] = Vertex_mark::Limit;
					break;
				}
			}
		}
	}

	return marking_map;
}

SM_marking_map mark_delimited_regions(Surface_mesh& M1, const SM_kd_tree& M2_tree, double threshold)
{
	mark_regions(M1, M2_tree, threshold);
	return mark_limits(M1);
}

SM_marking_map mark_delimited_regions(Surface_mesh& M1, const Surface_mesh& M2, double threshold)
{
	mark_regions(M1, M2, threshold);
	return mark_limits(M1);
}

template <class VertexRange>
auto marked_vertices(const Surface_mesh& mesh, const VertexRange& mesh_vertices,
					 const Vertex_mark mark)
{
	std::vector<Surface_mesh::Vertex_index> result;
	auto marking_map = get_marking_map(mesh);

	std::copy_if(mesh_vertices.begin(), mesh_vertices.end(), std::back_inserter(result),
				 [&, mark](auto v) { return marking_map[v] == mark; });

	return result;
}

auto none_vertices(const Surface_mesh& mesh)
{
	return marked_vertices(mesh, mesh.vertices(), Vertex_mark::None);
}

auto close_vertices(const Surface_mesh& mesh)
{
	return marked_vertices(mesh, mesh.vertices(), Vertex_mark::Close);
}

auto limit_vertices(const Surface_mesh& mesh)
{
	return marked_vertices(mesh, mesh.vertices(), Vertex_mark::Limit);
}

auto distant_vertices(const Surface_mesh& mesh)
{
	return marked_vertices(mesh, mesh.vertices(), Vertex_mark::Distant);
}
// template <class VertexRange>
// auto close_vertices(const VertexRange& vertices, const SM_marking_map& marking_map)
// {
// 	return marked_vertices(vertices, marking_map, Vertex_mark::Close);
// }
// template <class VertexRange>
// auto limit_vertices(const VertexRange& vertices, const SM_marking_map& marking_map)
// {
// 	return marked_vertices(vertices, marking_map, Vertex_mark::Limit);
// }

// template <class VertexRange>
// auto distant_vertices(const VertexRange& vertices, const SM_marking_map& marking_map)
// {
// 	return marked_vertices(vertices, marking_map, Vertex_mark::Distant);
// }

#endif // MESH_MARKING_INL