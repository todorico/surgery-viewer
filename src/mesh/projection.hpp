#ifndef MESH_PROJECTION_HPP
#define MESH_PROJECTION_HPP

// CGAL: Surface-mesh
// #include <CGAL/Simple_cartesian.h>
// #include <CGAL/Surface_mesh.h>

// CGAL: Projection
#include <CGAL/K_neighbor_search.h>
#include <CGAL/Kernel_traits.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>

#include <cmath>
// #include <CGAL/boost/iterator/counting_iterator.hpp>

// using Kernel = CGAL::Simple_cartesian<double>;
// // using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
// // using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;

// using Point	 = Kernel::Point_3;
// using Vector = Kernel::Vector_3;
// using Mesh	 = CGAL::Surface_mesh<Point>;

// class STDVector_property_map
// {
// 	const std::vector<Point>& points;

//   public:
// 	using value_type = Point;
// 	using reference	 = const value_type&;
// 	using key_type	 = size_t;
// 	using category	 = boost::lvalue_property_map_tag;

// 	STDVector_property_map(const std::vector<Point>& pts) : points(pts) {}

// 	reference operator[](key_type k) const
// 	{
// 		return points[k];
// 	}

// 	friend reference get(const STDVector_property_map& ppmap, key_type i)
// 	{
// 		return ppmap[i];
// 	}
// };

// using TreeTraits = CGAL::Search_traits_3<Kernel>;
// using TreeTraitsSTDVectorAdapter =
// 	CGAL::Search_traits_adapter<size_t, STDVector_property_map, TreeTraits>;
// using TreeTraitsMeshAdapter =
// 	CGAL::Search_traits_adapter<Mesh::Vertex_index,
// 								Mesh::Property_map<Mesh::Vertex_index, Mesh::Point>, TreeTraits>;

// using Neighbor_search = CGAL::K_neighbor_search<TreeTraitsMeshAdapter>;
// using Distance		  = Neighbor_search::Distance;
// using Tree			  = Neighbor_search::Tree;

template <class Vector>
Vector normalized(const Vector& v)
{
	return v / sqrt(v.squared_length());
}

struct WeightKernel
{
	enum WeightKernelType
	{
		Gaussian,
		Wendland,
		Singular,
		Uniform
	};

	enum WeightKernelRadiusMode
	{
		Constant,
		Max,
		Adaptive
	};

	WeightKernelType type;
	WeightKernelRadiusMode radiusMode;
	double radius;
	double sExponent;

	// Constant : r = radius
	// Max      : r = max(radius , max(r[i] , i \in neighbors))
	// Adaptive : r = max(r[i] , i \in neighbors)

	double weight(double d, double neihboringSphereRadius) const
	{
		double r = radiusMode == Constant
					   ? radius
					   : radiusMode == Max ? std::max<double>(radius, neihboringSphereRadius)
										   : neihboringSphereRadius;
		// kernel_type = 1 : Gaussien
		if(type == Gaussian)
		{
			return exp(-d * d / (r * r));
		}

		// kernel_type = 2 : Wendland
		else if(type == Wendland)
		{
			return pow(1 - d / r, 4) * (1 + 4 * d / r);
		}

		// kernel_type = 3 : Singulier
		else if(type == Singular)
		{
			return pow(r / d, sExponent);
		}

		// other : uniform
		else
		{
			return 1.0;
		}
	}
};

// faire attention au radius de WeightKernel
// template <class SurfaceMesh, class VertexRange>
// using Vector_3 = typename Kernel::Vector_3;

template <class Point, class KdTree, class NormalMap>
auto APSS(const Point& input_point, const KdTree& kd_tree, const NormalMap& normals,
		  const WeightKernel& weight_kernel = {WeightKernel::Gaussian, WeightKernel::Adaptive, 0,
											   0},
		  const size_t nb_iterations = 20, const unsigned int K = 20)
{
	using KeyType = typename KdTree::Point_d;
	using Kernel  = typename CGAL::Kernel_traits<Point>::Kernel;
	using Vector  = typename Kernel::Vector_3;

	// Initisalisation
	Vector output_normal;
	Vector output_point_v(input_point[0], input_point[1], input_point[2]);

	auto positions = kd_tree.traits().point_property_map();

	for(size_t i = 0; i < nb_iterations; i++)
	{
		// Find K nearest neighbors
		Point output_point_p(output_point_v[0], output_point_v[1], output_point_v[2]);

		Neighbor_search::Distance distance_adapter(kd_tree.traits().point_property_map());
		Neighbor_search search(kd_tree, output_point_p, K, 0, true, distance_adapter);

		double maxDist = sqrt((search.end() - 1)->second);

		double s_wi		= 0;
		double s_wipini = 0;
		double s_wipipi = 0;

		Vector s_wipi(0, 0, 0);
		Vector s_wini(0, 0, 0);

		for(auto point_dist_squared : search)
		{
			auto nni_idx	   = point_dist_squared.first;
			double nni_sqrDist = point_dist_squared.second;
			double wi		   = weight_kernel.weight(sqrt(nni_sqrDist), maxDist);

			Vector position(positions[nni_idx][0], positions[nni_idx][1], positions[nni_idx][2]);
			Vector normal(normals[nni_idx]);

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
		Vector u123 = (s_wini - 2 * u4 * s_wipi) / s_wi;
		double u0	= -(CGAL::scalar_product(s_wipi, u123) + u4 * s_wipipi) / s_wi;

		if(fabs(u4) < 0.000000000001)
		{
			// then project on a plane (it's a degenerate sphere)
			Vector n	  = -u123;
			double lambda = (u0 - CGAL::scalar_product(output_point_v, n)) / n.squared_length();
			output_point_v += lambda * n;
			output_normal = s_wini;
			output_normal = normalized(output_normal);
		}
		else
		{
			Vector sphere_center = u123 / (-2 * u4);
			double sphere_radius =
				sqrt(std::max<double>(0.0, sphere_center.squared_length() - u0 / u4));

			// projection of the inputpoint onto the sphere

			// direction of the point from the sphere center
			Vector pc = output_point_v - sphere_center;
			pc		  = normalized(pc);

			output_point_v = sphere_center + sphere_radius * pc;
			output_normal  = u123 + 2 * u4 * output_point_v;
			output_normal  = normalized(output_normal);
		}
	}

	Point output_point_p(output_point_v[0], output_point_v[1], output_point_v[2]);

	return {output_point_p, output_normal};
}

// Mesh projection(const Mesh& mesh, const std::vector<Mesh::Vertex_index>& mesh_vrange,
// 				const Tree& kd_tree, const Mesh::Property_map<Mesh::Vertex_index, Vector>& normals)
// {
// 	Mesh result = mesh;

// 	// auto [result_normal, created] = result.add_property_map<Mesh::Vertex_index,
// 	// Vector>("v:normal");

// 	// if(created)
// 	// std::cerr << "result_map created\n";

// 	for(auto vi : mesh_vrange)
// 	{
// 		auto [point, normal] = APSS(result.point(vi), kd_tree, normals);
// 		result.point(vi)	 = point;
// 		// result_normal[vi]	 = normal;
// 	}

// 	return result;
// }

template <class SurfaceMesh, class VertexRange, class PointMap, class NormalMap>
SurfaceMesh projection(const SurfaceMesh& mesh, const VertexRange& vertices,
					   const PointMap& point_map, const NormalMap& normal_map)
{
	using PointType	  = typename SurfaceMesh::Point;
	using VertexIndex = typename SurfaceMesh::Vertex_index;
	using Kernel	  = typename CGAL::Kernel_traits<PointType>::Kernel;

	using TreeTraits			= CGAL::Search_traits_3<Kernel>;
	using TreeTraitsMeshAdapter = CGAL::Search_traits_adapter<VertexIndex, PointMap, TreeTraits>;

	using Tree = CGAL::K_neighbor_search<TreeTraitsMeshAdapter>::Tree;

	Tree kd_tree(vertices.begin(), vertices.end(), Tree::Splitter(),
				 TreeTraitsMeshAdapter(point_property_map));

	SurfaceMesh result = mesh;

	for(auto v : vertices)
	{
		auto [point, normal] = APSS(result.point(v), kd_tree, normals);
		result.point(v)		 = point;
		// result_normal[vi]	 = normal;
	}

	return result;
}

template <class SurfaceMesh, class VertexRange>
SurfaceMesh projection(const SurfaceMesh& mesh, const VertexRange& vertices,
					   const SurfaceMesh& mesh2)
{
	using PointType	  = typename SurfaceMesh::Point;
	using VertexIndex = typename SurfaceMesh::Vertex_index;
	using Kernel	  = typename CGAL::Kernel_traits<PointType>::Kernel;

	auto [normal_map, exist] = result.template property_map<Vertex_index, Kernel::Vector_3>property_map("v:normal");

	if(exist)
	{
 		std::cerr << "[ERROR] projection : mesh normal map does not exist\n";
 		exit(EXIT_FAILURE);
	}

	return projection(mesh, vertices, mesh2.points());
}

// template <class Point, class KdTree, class NormalMap>
// Mesh projection(const Point& p, const Tree& kd_tree,
// 				const Mesh::Property_map<Mesh::Vertex_index, Vector>& normals)
// {
// 	Mesh result = mesh;

// 	// auto [result_normal, created] = result.add_property_map<Mesh::Vertex_index,
// 	// Vector>("v:normal");

// 	// if(created)
// 	// std::cerr << "result_map created\n";

// 	for(auto vi : mesh_vrange)
// 	{
// 		auto [point, normal] = APSS(result.point(vi), kd_tree, normals);
// 		result.point(vi)	 = point;
// 		// result_normal[vi]	 = normal;
// 	}

// 	return result;
// }

// Mesh projection(const Mesh& mesh1, const std::vector<Mesh::Vertex_index>& mesh1_vrange,
// 				const Mesh& mesh2, const Mesh::Vertex_range& mesh2_vrange)
// {
// 	Mesh copy_m2 = mesh2;
// 	// Calculating normals
// 	auto [mesh2_normal_map, mesh2_created] =
// 		copy_m2.add_property_map<Mesh::Vertex_index, Vector>("v:normal");

// 	for(auto vi : mesh2_vrange)
// 	{
// 		mesh2_normal_map[vi] = CGAL::Polygon_mesh_processing::compute_vertex_normal(vi, copy_m2);
// 	}
// 	// CGAL::Polygon_mesh_processing::compute_vertex_normals(copy_m2, mesh2_normal_map);

// 	// Kd-tree indexation
// 	Tree kd_tree(mesh2_vrange.begin(), mesh2_vrange.end(), Tree::Splitter(),
// 				 TreeTraitsMeshAdapter(copy_m2.points()));

// 	return projection(mesh1, mesh1_vrange, kd_tree, mesh2_normal_map);
// }

#include "projection.inl"

#endif // MESH_PROJECTION_HPP