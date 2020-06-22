#ifndef MESH_UTILS_INL
#define MESH_UTILS_INL

// CGAL
#include <CGAL/K_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Simple_cartesian.h>

// CGAL: Euler
#include <CGAL/boost/graph/Euler_operations.h>
#include <CGAL/boost/graph/iterator.h>

// STD
#include <limits>
#include <random>
#include <vector>

// CGAL::Kernel_traits< T >
// Point / size_type
// CGAL::Surface_mesh< P >::Vertex_range
// BidirectionalRange

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	band_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double min, double max)
{
	using TreeTraits	  = CGAL::Search_traits_3<CGAL::Simple_cartesian<double>>;
	using Distance		  = CGAL::Euclidean_distance<TreeTraits>;
	using Neighbor_search = CGAL::K_neighbor_search<TreeTraits, Distance>;
	using Tree			  = typename Neighbor_search::Tree;

	std::vector<typename CGAL::Surface_mesh<P>::Vertex_index> in_dist_vertices;
	Tree tree(M2.points().begin(), M2.points().end());

	// Iteration sur les index de points de M1 pour plus d'efficacité
	for(auto vertex_index : M1.vertices())
	{
		const unsigned int N = 1;

		Neighbor_search search(tree, M1.point(vertex_index), N);

		for(auto point_dist_squared : search)
		{
			double dist = std::sqrt(point_dist_squared.second);

			if(dist >= min && dist <= max)
				in_dist_vertices.push_back(vertex_index);
		}
	}

	return in_dist_vertices;
}

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	band_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double max)
{
	return band_pass_filter_dist(M1, M2, 0, max);
}

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	band_stop_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double min, double max)
{
	using TreeTraits	  = CGAL::Search_traits_3<CGAL::Simple_cartesian<double>>;
	using Distance		  = CGAL::Euclidean_distance<TreeTraits>;
	using Neighbor_search = CGAL::K_neighbor_search<TreeTraits, Distance>;
	using Tree			  = typename Neighbor_search::Tree;

	std::vector<typename CGAL::Surface_mesh<P>::Vertex_index> in_dist_vertices;
	Tree tree(M2.points().begin(), M2.points().end());

	// Iteration sur les index de points de M1 pour plus d'efficacité
	for(auto vertex_index : M1.vertices())
	{
		const unsigned int N = 1;
		Neighbor_search search(tree, M1.point(vertex_index), N);

		for(auto point_dist_squared : search)
		{
			double dist = std::sqrt(point_dist_squared.second);

			if(dist < min || dist > max)
				in_dist_vertices.push_back(vertex_index);
		}
	}

	return in_dist_vertices;
}

template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	band_stop_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double max)
{
	return band_stop_filter_dist(M1, M2, 0, max);
}

// Renvoie les indexes des points de M1 dont la distance la plus proche de M2 est inferieur a
// threshold
template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	low_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						 double max)
{
	return band_pass_filter_dist(M1, M2, 0, max);
}

// Renvoie les indexes des points de M1 dont la distance la plus proche de M2 est superieure ou egal
// à threshold
template <class P>
std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>
	high_pass_filter_dist(const CGAL::Surface_mesh<P>& M1, const CGAL::Surface_mesh<P>& M2,
						  double min)
{
	return band_pass_filter_dist(M1, M2, min, std::numeric_limits<double>::max());
}

template <class SurfaceMesh, class VertexRange>
void filter_out(SurfaceMesh& mesh, const VertexRange& vertices)
{
	for(auto vertex_index : vertices)
	{
		// Le sommet à potientiellement pu être effacer par un remove_face precedant
		// On passe au prochain sommet pour eviter les erreurs
		if(mesh.is_removed(vertex_index))
			continue;

		std::vector<typename SurfaceMesh::Halfedge_index> low_dist_halfhedges;

		auto halfedges = CGAL::halfedges_around_target(vertex_index, mesh);

		// Sans cette etapes remove_face plantera pendant l'iteration (on efface des
		// halfedge en iterant de)
		for(auto h : halfedges)
		{
			// if(!mesh.is_border(h))
			low_dist_halfhedges.push_back(h);
		}

		for(auto h : low_dist_halfhedges)
		{
			// Il est necessaire de re-checker à chaque fois car remove_face change l'état
			// du maillage
			if(!mesh.is_border(h))
				CGAL::Euler::remove_face(h, mesh);
		}
	}
}

// COLOR

CGAL::Color random_color()
{
	std::random_device rd;
	CGAL::Random random(rd());
	return CGAL::get_random_color(random);
}

template <class SurfaceMesh>
void set_mesh_color(SurfaceMesh& mesh, const CGAL::Color& color)
{
	auto [color_map, created] =
		mesh.template add_property_map<typename SurfaceMesh::Vertex_index, CGAL::Color>("v:color");

	if(created)
		std::cerr << "color_map created\n";
	else
		std::cerr << "color_map not created\n";

	for(auto vi : mesh.vertices())
	{
		color_map[vi] = color;
	}
}

/////////////////// MARKED

template <class SurfaceMesh, class Tree>
typename SurfaceMesh::template Property_map<typename SurfaceMesh::Vertex_index, Vertex_mark>
	marking(SurfaceMesh& M1, const Tree& M2_tree, double threshold)
{
	auto [marking_map, created] =
		M1.template add_property_map<typename SurfaceMesh::Vertex_index, Vertex_mark>(
			"v:mark", Vertex_mark::none);

	for(auto v : M1.vertices())
	{
		CGAL::K_neighbor_search<typename Tree::Traits> search(M2_tree, M1.point(v), 1);

		double distance = std::sqrt(search.begin()->second);

		if(distance > threshold)
		{
			marking_map[v] = Vertex_mark::distant;
		}
		else
		{
			marking_map[v] = Vertex_mark::close;
		}
	}

	for(auto v : M1.vertices())
	{
		if(marking_map[v] == Vertex_mark::close)
		{
			auto around_vertices = CGAL::vertices_around_target(M1.halfedge(v), M1);

			for(auto i : around_vertices)
			{
				if(marking_map[i] == Vertex_mark::distant)
				{
					marking_map[v] = Vertex_mark::limit;
					break;
				}
			}
		}
	}

	return marking_map;
}

template <class SurfaceMesh>
typename SurfaceMesh::template Property_map<typename SurfaceMesh::Vertex_index, Vertex_mark>
	marking(SurfaceMesh& M1, const SurfaceMesh& M2, double threshold)
{
	using Kernel		  = typename CGAL::Kernel_traits<typename SurfaceMesh::Point>::Kernel;
	using TreeTraits	  = CGAL::Search_traits_3<Kernel>;
	using Neighbor_search = CGAL::K_neighbor_search<TreeTraits>;
	using Tree			  = typename Neighbor_search::Tree;

	Tree M2_tree(M2.points().begin(), M2.points().end());

	return marking(M1, M2_tree, threshold);
}

template <class VertexRange, class MarkingMap>
auto marked(const VertexRange& vertices, const MarkingMap& marking_map, const Vertex_mark mark)
{
	std::vector<typename boost::range_value<VertexRange>::type> result;

	std::copy_if(vertices.begin(), vertices.end(), std::back_inserter(result),
				 [&, mark](auto v) { return marking_map[v] == mark; });

	return result;
}

// DIVIDE

template <class SurfaceMesh>
std::pair<SurfaceMesh, SurfaceMesh> divide(
	const SurfaceMesh& mesh,
	typename SurfaceMesh::template Property_map<typename SurfaceMesh::Vertex_index, Vertex_mark>
		marking_map)
{
	SurfaceMesh M1_minus = mesh;

	filter_out(M1_minus, marked(M1_minus.vertices(), marking_map, Vertex_mark::distant));

	SurfaceMesh M1_plus = mesh;

	filter_out(M1_plus, marked(M1_plus.vertices(), marking_map, Vertex_mark::close));

	return {M1_minus, M1_plus};
}

// MERGE

// template <class SurfaceMesh, class VertexRange>
// SurfaceMesh merge(const SurfaceMesh& M1, const SurfaceMesh& M2)
// {

// }

///////////////////////////////////////////////////////////////////// MLS

#endif // MESH_UTILS_INL
