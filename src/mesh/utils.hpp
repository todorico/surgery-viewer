#ifndef MESH_UTILS_HPP
#define MESH_UTILS_HPP

#include <CGAL/Surface_mesh.h>
#include <vector>

// BAND-PASS

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	band_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double min, double max);

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	band_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double max);

// BAND-STOP

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	band_stop_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double min, double max);

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	band_stop_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double max);

// LOW/HIGH-PASS

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	low_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						 double max);

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	high_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double min);

// FILTER

template <class P>
void filter_out(CGAL::Surface_mesh<P>& mesh,
				const std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>& vertices);

#include "utils.inl"

#endif // MESH_UTILS_HPP
