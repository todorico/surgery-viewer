#ifndef MESH_UTILS_HPP
#define MESH_UTILS_HPP

#include "../instance/Surface_mesh.hpp"

#include "marking.hpp"

// CGAL
#include <CGAL/IO/Color.h>

// COLOR

CGAL::Color random_color();

template <class VertexRange>
void set_mesh_color(Surface_mesh& mesh, const VertexRange& vertices, const CGAL::Color& color);

void set_mesh_color(Surface_mesh& mesh, const CGAL::Color& color);

Surface_mesh translated(const Surface_mesh& mesh, const Kernel::Vector_3& v);

// FILTER

template <class VertexRange>
Surface_mesh filtered(const Surface_mesh& mesh, const VertexRange& vertices);

// // DIVIDE

std::pair<Surface_mesh, Surface_mesh> divide(const Surface_mesh& mesh);

// // BAND-PASS

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	band_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double min, double max);

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	band_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double max);

// // BAND-STOP

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	band_stop_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double min, double max);

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	band_stop_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double max);

// // LOW/HIGH-PASS

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	low_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						 double max);

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	high_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double min);

#include "utils.inl"

#endif // MESH_UTILS_HPP
