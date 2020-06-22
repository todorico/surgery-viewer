#ifndef MESH_UTILS_HPP
#define MESH_UTILS_HPP

// CGAL

#include <CGAL/IO/Color.h>
#include <CGAL/Surface_mesh.h>

// STD

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

template <class SurfaceMesh, class VertexRange>
void filter_out(SurfaceMesh& mesh, const VertexRange& vertices);

// COLOR

CGAL::Color random_color();

template <class SurfaceMesh>
void set_mesh_color(SurfaceMesh& mesh, const CGAL::Color& color);

// MARKING

enum class Vertex_mark : unsigned char
{
	none = 0,
	close,
	distant,
	limit
};

template <class SurfaceMesh, class Tree>
typename SurfaceMesh::template Property_map<typename SurfaceMesh::Vertex_index, Vertex_mark>
	marking(SurfaceMesh& M1, const Tree& M2_tree, double threshold);

template <class SurfaceMesh>
typename SurfaceMesh::template Property_map<typename SurfaceMesh::Vertex_index, Vertex_mark>
	marking(SurfaceMesh& M1, const SurfaceMesh& M2, double threshold);

template <class VertexRange, class MarkingMap>
auto marked(const VertexRange& vertices, const MarkingMap& marking_map, const Vertex_mark mark);

// DIVIDE

template <class SurfaceMesh>
std::pair<SurfaceMesh, SurfaceMesh> divide(
	const SurfaceMesh& mesh,
	typename SurfaceMesh::template Property_map<typename SurfaceMesh::Vertex_index, Vertex_mark>
		marking_map);

// PROJECT

template <class SurfaceMesh>
SurfaceMesh projection(const SurfaceMesh& M1, const SurfaceMesh& M2)
{
}

#include "utils.inl"

#endif // MESH_UTILS_HPP
