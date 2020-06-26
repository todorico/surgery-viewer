#ifndef MESH_UTILS_INL
#define MESH_UTILS_INL

#include "utils.hpp"

// // CGAL
// #include <CGAL/K_neighbor_search.h>
// #include <CGAL/Search_traits_3.h>
// #include <CGAL/Simple_cartesian.h>

// // CGAL: Euler
// #include <CGAL/boost/graph/Euler_operations.h>
// #include <CGAL/boost/graph/iterator.h>

// // STD
// #include <limits>
#include <random>
#include <vector>

// COLOR

CGAL::Color random_color()
{
	std::random_device rd;
	CGAL::Random random(rd());
	return CGAL::get_random_color(random);
}

template <class VertexRange>
void set_mesh_color(Surface_mesh& mesh, const VertexRange& vertices, const CGAL::Color& color)
{
	auto [color_map, created] =
		mesh.add_property_map<Surface_mesh::Vertex_index, CGAL::Color>("v:color");

	// if(created)
	// 	std::cerr << "color_map created\n";
	// else
	// 	std::cerr << "color_map not created\n";

	for(auto v : vertices)
	{
		color_map[v] = color;
	}
}

void set_mesh_color(Surface_mesh& mesh, const CGAL::Color& color)
{
	set_mesh_color(mesh, mesh.vertices(), color);
}

Surface_mesh translated(const Surface_mesh& mesh, const Kernel::Vector_3& v)
{
	Surface_mesh result = mesh;

	for(auto& p : result.points())
	{
		p = (p + v);
	}

	return result;
}

template <class VertexRange>
Surface_mesh filtered(const Surface_mesh& mesh, const VertexRange& vertices)
{
	Surface_mesh result = mesh;

	for(auto vertex_index : vertices)
	{
		// Le sommet à potientiellement pu être effacer par un remove_face precedant
		// On passe au prochain sommet pour eviter les erreurs
		if(result.is_removed(vertex_index))
			continue;

		std::vector<Surface_mesh::Halfedge_index> low_dist_halfhedges;

		auto halfedges = CGAL::halfedges_around_target(vertex_index, result);

		// Sans cette etapes remove_face plantera pendant l'iteration (on efface des
		// halfedge en iterant de)
		for(auto h : halfedges)
		{
			// if(!result.is_border(h))
			low_dist_halfhedges.push_back(h);
		}

		for(auto h : low_dist_halfhedges)
		{
			// Il est necessaire de re-checker à chaque fois car remove_face change l'état
			// du maillage
			if(!result.is_border(h))
				CGAL::Euler::remove_face(h, result);
		}
	}

	return result;
}

// DIVIDE

std::pair<Surface_mesh, Surface_mesh> divide(const Surface_mesh& mesh)
{
	Surface_mesh M1_close	= filtered(mesh, distant_vertices(mesh));
	Surface_mesh M1_distant = filtered(mesh, close_vertices(mesh));

	return {M1_close, M1_distant};
}

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	band_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double min, double max)
// {
// 	using TreeTraits	  = CGAL::Search_traits_3<CGAL::Simple_cartesian<double>>;
// 	using Distance		  = CGAL::Euclidean_distance<TreeTraits>;
// 	using Neighbor_search = CGAL::K_neighbor_search<TreeTraits, Distance>;
// 	using Tree			  = typename Neighbor_search::Tree;

// 	std::vector<typename CGAL::Surface_mesh<P>::Vertex_index> in_dist_vertices;
// 	Tree tree(M2.points().begin(), M2.points().end());

// 	// Iteration sur les index de points de M1 pour plus d'efficacité
// 	for(auto vertex_index : M1.vertices())
// 	{
// 		const unsigned int N = 1;

// 		Neighbor_search search(tree, M1.point(vertex_index), N);

// 		for(auto point_dist_squared : search)
// 		{
// 			double dist = std::sqrt(point_dist_squared.second);

// 			if(dist >= min && dist <= max)
// 				in_dist_vertices.push_back(vertex_index);
// 		}
// 	}

// 	return in_dist_vertices;
// }

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	band_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double max)
// {
// 	return band_pass_filter_dist(M1, M2, 0, max);
// }

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	band_stop_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double min, double max)
// {
// 	using TreeTraits	  = CGAL::Search_traits_3<CGAL::Simple_cartesian<double>>;
// 	using Distance		  = CGAL::Euclidean_distance<TreeTraits>;
// 	using Neighbor_search = CGAL::K_neighbor_search<TreeTraits, Distance>;
// 	using Tree			  = typename Neighbor_search::Tree;

// 	std::vector<typename CGAL::Surface_mesh<P>::Vertex_index> in_dist_vertices;
// 	Tree tree(M2.points().begin(), M2.points().end());

// 	// Iteration sur les index de points de M1 pour plus d'efficacité
// 	for(auto vertex_index : M1.vertices())
// 	{
// 		const unsigned int N = 1;
// 		Neighbor_search search(tree, M1.point(vertex_index), N);

// 		for(auto point_dist_squared : search)
// 		{
// 			double dist = std::sqrt(point_dist_squared.second);

// 			if(dist < min || dist > max)
// 				in_dist_vertices.push_back(vertex_index);
// 		}
// 	}

// 	return in_dist_vertices;
// }

// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	band_stop_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double max)
// {
// 	return band_stop_filter_dist(M1, M2, 0, max);
// }

// // Renvoie les indexes des points de M1 dont la distance la plus proche de M2 est inferieur a
// // threshold
// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	low_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						 double max)
// {
// 	return band_pass_filter_dist(M1, M2, 0, max);
// }

// // Renvoie les indexes des points de M1 dont la distance la plus proche de M2 est superieure ou
// egal
// // à threshold
// template <class P>
// std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
// 	high_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
// 						  double min)
// {
// 	return band_pass_filter_dist(M1, M2, min, std::numeric_limits<double>::max());
// }

#endif // MESH_UTILS_INL
