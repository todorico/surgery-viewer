#ifndef MESH_MARKING_HPP
#define MESH_MARKING_HPP

#include "../instance/Surface_mesh_kd_tree.hpp"

// Cette enumération est utilisée pour annoter les sommets d'un maillage
// - Close   -> Sommet proche d'un autre maillage
// - Distant -> Sommet distant d'un autre maillage
// - Limit   -> Sommet "proche" aillant au moins un voisins "distant"
enum class Vertex_mark : unsigned char
{
	None = 0,
	Close,
	Limit,
	Distant
};

// Type utilisé pour servir de carte d'annotation de distance sur nos maillages
using SM_marking_map =
	Surface_mesh::Property_map<Surface_mesh::Vertex_index, Vertex_mark>;

// Renvoie la carte d'annotation associée à un maillage (assertion failure si la carte n'existe pas)
SM_marking_map get_marking_map(const Surface_mesh& mesh);

// Créée et renvoie la carte d'annotation en fonction des distances entre 2 maillages (close / distant)
SM_marking_map mark_regions(Surface_mesh& M1, const SM_kd_tree& M2_tree,
							double threshold, double epsilon = 0);
SM_marking_map mark_regions(Surface_mesh& M1, const Surface_mesh& M2,
							double threshold, double epsilon = 0);

// Rajoute les une annotation sur sommets limites d'un maillages qui à déja été marqué (close / distant)
SM_marking_map mark_limits(const Surface_mesh& mesh);

// Change limits value with mark on mesh
SM_marking_map mark_limits_with(const Surface_mesh& mesh, const Vertex_mark& mark);


// Comme mark_regions mais en rajoutant les limites (close / distant / limites)
// Equivalent à un appel de mark_regions + mark_limits
SM_marking_map mark_delimited_regions(Surface_mesh& M1,
									  const SM_kd_tree& M2_tree,
									  double threshold, double epsilon = 0);
SM_marking_map mark_delimited_regions(Surface_mesh& M1, const Surface_mesh& M2,
									  double threshold, double epsilon = 0);


// Renvoie les index des sommets qui ont une annotation 'mark' associée
template <class VertexRange>
auto marked_vertices(const Surface_mesh& mesh, const VertexRange& mesh_vertices,
					 const Vertex_mark mark);
auto marked_vertices(const Surface_mesh& mesh, const Vertex_mark& mark);


// Renvoie les index des sommets qui ont une annotation 'mark' associée et qui ont au moins un voisin avec une annotation 
// mark_neighbor
template <class VertexRange>
auto marked_vertices(const Surface_mesh& mesh, const VertexRange& mesh_vertices,
                     const Vertex_mark mark, const Vertex_mark mark_neighbor);
auto marked_vertices(const Surface_mesh& mesh,
                     const Vertex_mark mark, const Vertex_mark mark_neighbor);

// Surcharge de marked_vertices pour être plus simple d'utilisation
auto none_vertices(const Surface_mesh& mesh);

auto close_vertices(const Surface_mesh& mesh);

auto limit_vertices(const Surface_mesh& mesh);

auto distant_vertices(const Surface_mesh& mesh);

#include "marking.inl"

#endif // MESH_MARKING_HPP