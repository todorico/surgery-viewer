#ifndef MESH_PROJECTION_HPP
#define MESH_PROJECTION_HPP

#include "../instance/Surface_mesh_kd_tree.hpp"

struct Weight_kernel
{
	enum class Type
	{
		Gaussian,
		Wendland,
		Singular,
		Uniform
	};

	enum Mode
	{
		Constant,
		Max,
		Adaptive
	};

	Type type		 = Type::Gaussian;
	Mode radius_mode = Mode::Adaptive;

	double radius	  = 0;
	double s_exponent = 0;

	// Constant : r = radius
	// Max      : r = max(radius , max(r[i] , i \in neighbors))
	// Adaptive : r = max(r[i] , i \in neighbors)

	double weight(double d, double neihboringSphereRadius) const;
};

Kernel::Vector_3 normalized(const Kernel::Vector_3& v);

std::pair<Kernel::Point_3, Kernel::Vector_3>
	APSS(const Kernel::Point_3& input_point, const SM_kd_tree& kd_tree,
		 const Surface_mesh_normal_map& normals, const size_t nb_iterations = 20,
		 const unsigned int K				= 20,
		 const Weight_kernel& weight_kernel = {Weight_kernel::Type::Gaussian,
											   Weight_kernel::Mode::Adaptive, 0, 0});

template <class VertexRange>
Surface_mesh projection(const Surface_mesh& mesh, const VertexRange& vertices,
						const SM_kd_tree& points, const Surface_mesh_normal_map& normals);

template <class VertexRange1, class VertexRange2>
Surface_mesh projection(const Surface_mesh& M1, const VertexRange1& M1_vertices,
						const Surface_mesh& M2, const VertexRange2& M2_vertices);

Surface_mesh projection(const Surface_mesh& M1, const Surface_mesh& M2);

#include "projection.inl"

#endif // MESH_PROJECTION_HPP