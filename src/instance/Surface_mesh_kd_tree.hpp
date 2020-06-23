#ifndef INSTANCE_SURFACE_MESH_KD_TREE_HPP
#define INSTANCE_SURFACE_MESH_KD_TREE_HPP

#include "Surface_mesh.hpp"

// CGAL
#include <CGAL/K_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>

using SM_kd_tree_traits			= CGAL::Search_traits_3<Kernel>;
using SM_kd_tree_traits_adapter = CGAL::Search_traits_adapter<
	Surface_mesh::Vertex_index,
	Surface_mesh::Property_map<Surface_mesh::Vertex_index, Surface_mesh::Point>, SM_kd_tree_traits>;

using SM_kd_tree_search	  = CGAL::K_neighbor_search<SM_kd_tree_traits_adapter>;
using SM_kd_tree		  = SM_kd_tree_search::Tree;
using SM_kd_tree_distance = SM_kd_tree_search::Distance;
using SM_kd_tree_splitter = SM_kd_tree_search::Splitter;

#endif // INSTANCE_SURFACE_MESH_KD_TREE_HPP