#ifndef INSTANCE_SURFACE_MESH_HPP
#define INSTANCE_SURFACE_MESH_HPP

#include "Kernel.hpp"

// CGAL
#include <CGAL/Surface_mesh.h>

using Surface_mesh = CGAL::Surface_mesh<Kernel::Point_3>;

using Surface_mesh_point_map =
	Surface_mesh::Property_map<Surface_mesh::Vertex_index, Surface_mesh::Point>;

using Surface_mesh_normal_map =
	Surface_mesh::Property_map<Surface_mesh::Vertex_index, Kernel::Vector_3>;

#endif // INSTANCE_SURFACE_MESH_HPP