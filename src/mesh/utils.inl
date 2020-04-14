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
#include <vector>

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

template <class P>
void filter_out(CGAL::Surface_mesh<P>& mesh,
				const std::vector<typename CGAL::Surface_mesh<P>::Vertex_index>& vertices)
{
	for(auto vertex_index : vertices)
	{
		// Le sommet à potientiellement pu être effacer par un remove_face precedant
		// On passe au prochain sommet pour eviter les erreurs
		if(mesh.is_removed(vertex_index))
			continue;

		std::vector<typename CGAL::Surface_mesh<P>::Halfedge_index> low_dist_halfhedges;
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

///////////////////////////////////////////////////////////////////// MLS

#endif // MESH_UTILS_INL

///////////////////////// MANIPULATORS

/*
void print_v(const Mesh& M)
{
	for(auto vertex : M.vertices())
		std::cerr << vertex << ": " << M.point(vertex) << '\n';
}

void print_e(const Mesh& M)
{
	for(auto edge : M.edges())
		std::cerr << edge << ": (" << M.vertex(edge, 0) << ") -- (" << M.vertex(edge, 1) << ")\n";
}

void operate_v(Mesh& m, Mesh::Vertex_index vi, std::string query)
{
	if(query == "rm")
	{
		m.remove_vertex(vi);
	}
	else if(query == "rmc")
	{
		// Need no incident face to vi is a hole
		// Need min 2 faces incident to faces that are incident to vi
		auto halfedge	 = m.halfedge(vi);
		auto res		  = CGAL::Euler::remove_center_vertex(halfedge, m);
		auto res_opposite = m.opposite(res);
		std::cerr << "res\t\t= " << res << '\n';
		std::cerr << "is_border(" << res << ")\t= " << m.is_border(res) << '\n';
		std::cerr << "is_border(opposite(" << res << "))\t= " << m.is_border(res_opposite) << '\n';
	}
	else if(query == "rf")
	{
		// Attention invalidation des halfedges
		std::vector<Mesh::Halfedge_index> to_remove;
		// auto incident = m.halfedge(vi);

		auto halfedges = CGAL::halfedges_around_target(vi, m);

		for(auto h : halfedges)
			to_remove.push_back(h);

		for(auto h : to_remove)
		{
			if(!m.is_valid(h) && !m.is_removed(h))
				continue;

			if(!m.is_border(h))
			{
				CGAL::Euler::remove_face(h, m);
			}
			else
			{
				std::cerr << "ignored border halfedge\n";
			}
		}
	}
	else
	{
		std::cerr << "Error: unsupported operation " << query << "on vertex\n";
	}
}

void operate_h(Mesh& m, Mesh::Halfedge_index hi, std::string query)
{
	if(query == "mh")
	{
		// Need no incident border edge
		CGAL::Euler::make_hole(hi, m);
	}
	else if(query == "rf")
	{
		// Need h not a border halfedge
		CGAL::Euler::remove_face(hi, m);
	}
	else
	{
		std::cerr << "Error: unsupported operation " << query << "on halfedge\n";
	}
}

void operate_e(Mesh& m, Mesh::Edge_index ei, std::string query)
{
	if(query == "rm")
	{
		m.remove_edge(ei);
	}
	else
	{
		std::cerr << "Error: unsupported operation " << query << "on edge\n";
	}
}

void operate_f(Mesh& m, Mesh::Face_index fi, std::string query)
{
	if(query == "rm")
	{
		m.remove_face(fi);
	}
	else
	{
		std::cerr << "Error: unsupported operation " << query << "on face\n";
	}
}

void operate_m(Mesh& m, std::string query)
{
	if(query == "print")
	{
		print_e(m);
		print_v(m);
	}
	else if(query == "draw")
	{
		CGAL::draw(m);
	}
	else
	{
		std::cerr << "Error: unsupported operation " << query << "on mesh\n";
	}
}

void operate(Mesh& m, std::string element_id, std::string query)
{
	char type		   = element_id.front();
	Mesh::size_type id = element_id.size() > 1 ? stoul(element_id.substr(1)) : size_t(0);

	switch(type)
	{
		case 'v':
			operate_v(m, Mesh::Vertex_index(id), query);
			break;
		case 'h':
			operate_h(m, Mesh::Halfedge_index(id), query);
			break;
		case 'e':
			operate_e(m, Mesh::Edge_index(id), query);
			break;
		case 'f':
			operate_f(m, Mesh::Face_index(id), query);
			break;
		case 'm':
			operate_m(m, query);
			break;
		default:
			std::cerr << "Error: unsupported object type: " << type << " from " << element_id
					  << '\n';
			return;
			break;
	}
}

void print_state(Mesh& mesh)
{
	std::cerr << "number_of_removed_halfedges : " << mesh.number_of_removed_halfedges() << '\n';
	std::cerr << "number_of_removed_faces     : " << mesh.number_of_removed_faces() << '\n';
	std::cerr << "number_of_removed_vertices  : " << mesh.number_of_removed_vertices() << '\n';
	std::cerr << "number_of_removed_edges     : " << mesh.number_of_removed_edges() << '\n';
}
*/
