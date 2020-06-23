#ifndef MESH_MARKING_HPP
#define MESH_MARKING_HPP

#include "../instance/Surface_mesh_kd_tree.hpp"

enum class Vertex_mark : unsigned char
{
	None = 0,
	Close,
	Limit,
	Distant
};

using SM_marking_map = Surface_mesh::Property_map<Surface_mesh::Vertex_index, Vertex_mark>;

// return Surface_mesh::Property_map<Surface_mesh::Vertex_index, Vertex_mark>
SM_marking_map mark_regions(Surface_mesh& M1, const SM_kd_tree& M2_tree, double threshold);
SM_marking_map mark_regions(Surface_mesh& M1, const Surface_mesh& M2, double threshold);

SM_marking_map mark_limits(const Surface_mesh& M1, SM_marking_map& mark_map);

SM_marking_map mark_delimited_regions(Surface_mesh& M1, const SM_kd_tree& M2_tree,
									  double threshold);
SM_marking_map mark_delimited_regions(Surface_mesh& M1, const Surface_mesh& M2, double threshold);

template <class VertexRange>
auto marked_vertices(const VertexRange& vertices, const SM_marking_map& marking_map,
					 const Vertex_mark mark);

template <class VertexRange>
auto none_vertices(const VertexRange& vertices, const SM_marking_map& marking_map);

template <class VertexRange>
auto close_vertices(const VertexRange& vertices, const SM_marking_map& marking_map);

template <class VertexRange>
auto limit_vertices(const VertexRange& vertices, const SM_marking_map& marking_map);

template <class VertexRange>
auto distant_vertices(const VertexRange& vertices, const SM_marking_map& marking_map);

#include "marking.inl"

#endif // MESH_MARKING_HPP