#ifndef MESH_UTILS_HPP
#define MESH_UTILS_HPP

#include "../instance/Surface_mesh.hpp"

#include "marking.hpp"

// CGAL
#include <CGAL/IO/Color.h>

// COLOR

std::array<float, 4> random_color();

template <class VertexRange>
void set_mesh_color(Surface_mesh& mesh, const VertexRange& vertices,
					const std::array<float, 4>& color);

void set_mesh_color(Surface_mesh& mesh, const std::array<float, 4>& color);

// Applique une translation sur tout les sommets d'un maillage.
Surface_mesh translated(const Surface_mesh& mesh, const Kernel::Vector_3& v);

// FILTER

// Enlève les sommets 'vertices' du maillages 'mesh'
template <class VertexRange>
Surface_mesh filtered(const Surface_mesh& mesh, const VertexRange& vertices);

// // DIVIDE

// Divise un maillage en deux.
// Precondition : le maillage mesh doit avoir ses sommets annotés 'close/distant' (cf. marking.hpp).
// si vous voulez dupliquez les sommets limites rajoutez l'annotation 'limit' sur les sommets.
std::pair<Surface_mesh, Surface_mesh> divide(const Surface_mesh& mesh);

#include "utils.inl"

#endif // MESH_UTILS_HPP
