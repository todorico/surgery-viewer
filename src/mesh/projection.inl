#ifndef MESH_PROJECTION_INL
#define MESH_PROJECTION_INL

#include "projection.hpp"

#include <cmath>

double Weight_kernel::weight(double d, double neihboringSphereRadius) const
{
	double r = radius_mode == Mode::Constant
				   ? radius
				   : radius_mode == Mode::Max ? std::max<double>(radius, neihboringSphereRadius)
											  : neihboringSphereRadius;
	if(type == Type::Gaussian)
		return std::exp(-d * d / (r * r));
	else if(type == Type::Wendland)
		return std::pow(1 - d / r, 4) * (1 + 4 * d / r);
	else if(type == Type::Singular)
		return std::pow(r / d, s_exponent);
	else
		return 1.0;
}

Kernel::Vector_3 normalized(const Kernel::Vector_3& v)
{
	return v / std::sqrt(v.squared_length());
}

std::pair<Kernel::Point_3, Kernel::Vector_3> APSS(const Kernel::Point_3& input_point,
												  const SM_kd_tree& kd_tree,
												  const Surface_mesh_normal_map& normals,
												  const size_t nb_iterations, const unsigned int K,
												  const Weight_kernel& weight_kernel)
{
	// Initisalisation
	Kernel::Vector_3 output_normal;
	Kernel::Vector_3 output_point_v(input_point[0], input_point[1], input_point[2]);

	auto positions = kd_tree.traits().point_property_map();

	for(size_t i = 0; i < nb_iterations; i++)
	{
		// Find K nearest neighbors
		Kernel::Point_3 output_point_p(output_point_v[0], output_point_v[1], output_point_v[2]);

		SM_kd_tree_search search(kd_tree, output_point_p, K, 0, true,
								 kd_tree.traits().point_property_map());

		double maxDist = std::sqrt((search.end() - 1)->second);

		double s_wi		= 0;
		double s_wipini = 0;
		double s_wipipi = 0;

		Kernel::Vector_3 s_wipi(0, 0, 0);
		Kernel::Vector_3 s_wini(0, 0, 0);

		for(auto point_dist_squared : search)
		{
			auto nni_idx	   = point_dist_squared.first;
			double nni_sqrDist = point_dist_squared.second;
			double wi		   = weight_kernel.weight(std::sqrt(nni_sqrDist), maxDist);

			Kernel::Vector_3 position(positions[nni_idx][0], positions[nni_idx][1],
									  positions[nni_idx][2]);
			Kernel::Vector_3 normal(normals[nni_idx]);

			s_wipini += (wi * CGAL::scalar_product(position, normal));
			s_wipipi += (wi * CGAL::scalar_product(position, normal));

			s_wipi += (wi * position);
			s_wini += (wi * normal);
			s_wi += wi;
		}

		// algebraic sphere: u4.||X||^2 + u123.X + u0 = 0
		// geometric sphere: ||X-C||^2 - r^2 = 0
		// geometric plane:  (X-C).n = 0
		double u4 = 0.5 * (s_wipini / s_wi - CGAL::scalar_product(s_wipi / s_wi, s_wini / s_wi)) /
					(s_wipipi / s_wi - CGAL::scalar_product(s_wipi / s_wi, s_wipi / s_wi));
		Kernel::Vector_3 u123 = (s_wini - 2 * u4 * s_wipi) / s_wi;
		double u0			  = -(CGAL::scalar_product(s_wipi, u123) + u4 * s_wipipi) / s_wi;

		if(fabs(u4) < 0.000000000001)
		{
			// then project on a plane (it's a degenerate sphere)
			Kernel::Vector_3 n = -u123;
			double lambda = (u0 - CGAL::scalar_product(output_point_v, n)) / n.squared_length();
			output_point_v += lambda * n;
			output_normal = s_wini;
			output_normal = normalized(output_normal);
		}
		else
		{
			Kernel::Vector_3 sphere_center = u123 / (-2 * u4);
			double sphere_radius =
				std::sqrt(std::max<double>(0.0, sphere_center.squared_length() - u0 / u4));

			// projection of the inputpoint onto the sphere

			// direction of the point from the sphere center
			Kernel::Vector_3 pc = output_point_v - sphere_center;
			pc					= normalized(pc);

			output_point_v = sphere_center + sphere_radius * pc;
			output_normal  = u123 + 2 * u4 * output_point_v;
			output_normal  = normalized(output_normal);
		}
	}

	Kernel::Point_3 output_point_p(output_point_v[0], output_point_v[1], output_point_v[2]);

	return {output_point_p, output_normal};
}

template <class VertexRange>
Surface_mesh projection(const Surface_mesh& mesh, const VertexRange& vertices,
						const SM_kd_tree& points, const Surface_mesh_normal_map& normals)
{
	Surface_mesh result = mesh;

	// auto [result_normal, created] = result.add_property_map<Mesh::Vertex_index,
	// Vector>("v:normal");

	// if(created)
	// std::cerr << "result_map created\n";

	for(auto v : vertices)
	{
		auto [point, normal] = APSS(result.point(v), points, normals);
		result.point(v)		 = point;
		// result_normal[vi]	 = normal;
	}

	return result;
}

template <class VertexRange1, class VertexRange2>
Surface_mesh projection(const Surface_mesh& M1, const VertexRange1& M1_vertices,
						const Surface_mesh& M2, const VertexRange2& M2_vertices)
{
	// Surface_mesh copy_m2 = M2;

	// Calculating normals
	auto [M2_normal_map, M2_exist] =
		M2.property_map<Surface_mesh::Vertex_index, Kernel::Vector_3>("v:normal");

	if(!M2_exist)
	{
		// for(auto v : M2_vertices)
		// {
		// 	M2_normal_map[v] = CGAL::Polygon_mesh_processing::compute_vertex_normal(v, copy_m2);
		// }

		std::cerr << "[ERROR] in projection M2 do not have a vertex normal map\n";
		exit(EXIT_FAILURE);
	}

	// Kd-tree indexation
	SM_kd_tree kd_tree(M2_vertices.begin(), M2_vertices.end(), SM_kd_tree_splitter(),
					   SM_kd_tree_traits_adapter(M2.points()));

	return projection(M1, M1_vertices, kd_tree, M2_normal_map);
}

Surface_mesh projection(const Surface_mesh& M1, const Surface_mesh& M2)
{
	return projection(M1, M1.vertices(), M2, M2.vertices());
}

#endif // MESH_PROJECTION_INL