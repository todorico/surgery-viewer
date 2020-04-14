// Project
#include "docopt/docopt.h"
#include "mesh/draw.hpp"
#include "mesh/io.hpp"
// #include "mesh/utils.hpp"

// CGAL: Surface-mesh
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>

#include <CGAL/Polygon_mesh_processing/compute_normal.h>

// STD
#include <iostream>
#include <random>
#include <string>

using Kernel = CGAL::Simple_cartesian<double>;
// using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
// using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;

using Point	 = Kernel::Point_3;
using Vector = Kernel::Vector_3;
using Mesh	 = CGAL::Surface_mesh<Point>;

#include <CGAL/K_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/boost/iterator/counting_iterator.hpp>
// definition of a non-mutable lvalue property map,
// with the get function as a friend function to give it
// access to the private member
class My_point_property_map
{
	const std::vector<Point>& points;

  public:
	using value_type = Point;
	using reference	 = const value_type&;
	using key_type	 = size_t;
	using category	 = boost::lvalue_property_map_tag;

	My_point_property_map(const std::vector<Point>& pts) : points(pts) {}

	reference operator[](key_type k) const
	{
		return points[k];
	}

	friend reference get(const My_point_property_map& ppmap, key_type i)
	{
		return ppmap[i];
	}
};

using TreeTraits = CGAL::Search_traits_3<Kernel>;
using TreeTraitsArrayAdapter =
	CGAL::Search_traits_adapter<size_t, My_point_property_map, TreeTraits>;
using TreeTraitsMeshAdapter =
	CGAL::Search_traits_adapter<Mesh::Vertex_index,
								Mesh::Property_map<Mesh::Vertex_index, Mesh::Point>, TreeTraits>;

using Neighbor_search = CGAL::K_neighbor_search<TreeTraitsMeshAdapter>;
using Distance		  = Neighbor_search::Distance;
using Tree			  = Neighbor_search::Tree;

Vector normalized(const Vector& v)
{
	return v / sqrt(v.squared_length());
}

Mesh translated(const Mesh& mesh, const Vector& v)
{
	Mesh res = mesh;

	for(auto vi : res.vertices())
	{
		res.point(vi) += v;
	}

	return res;
}

CGAL::Color random_color()
{
	std::random_device rd;
	CGAL::Random random(rd());
	return CGAL::get_random_color(random);
}

CGAL::Color color_function(const CGAL::Color& c1, const CGAL::Color& c2)
{
	CGAL::Color res;
	for(size_t i = 0; i < 4; ++i)
	{
		double c = (c1[i] + c2[i]) / 2.0;
		res[i]	 = static_cast<unsigned char>(c);
	}
	return res;
}

void set_mesh_color(Mesh& mesh, const CGAL::Color& color)
{
	auto [color_map, created] = mesh.add_property_map<Mesh::Vertex_index, CGAL::Color>("v:color");

	if(created)
		std::cerr << "color_map created\n";
	else
		std::cerr << "color_map not created\n";

	for(auto vi : mesh.vertices())
	{
		color_map[vi] = color;
	}
}

// Print STL containers in a generic way

template <typename T>
std::ostream& print_container_element(std::ostream& out, const T& elem)
{
	return (out << elem << " ");
}

template <typename T1, typename T2>
std::ostream& print_container_element(std::ostream& out, const std::pair<T1, T2>& elem)
{
	return (out << "{" << elem.first << " " << elem.second << "} ");
}

template <template <typename, typename...> class TT, typename... Args>
std::ostream& operator<<(std::ostream& out, TT<Args...> const& container)
{
	out << "[ ";

	for(auto& elem : container)
		print_container_element(out, elem);

	return out << "]";
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
std::pair<Point, Vector> APSS(const Point& input_point, const Tree& kd_tree,
							  const Mesh::Property_map<Mesh::Vertex_index, Vector>& normals,
							  const WeightKernel& weight_kernel = {WeightKernel::Gaussian,
																   WeightKernel::Adaptive, 0, 0},
							  const size_t nb_iterations = 10, const unsigned int K = 20)
{
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

Mesh projection(const Mesh& mesh, const Tree& kd_tree,
				const Mesh::Property_map<Mesh::Vertex_index, Vector>& normals)
{
	Mesh result = mesh;

	auto [result_normal, created] = result.add_property_map<Mesh::Vertex_index, Vector>("v:normal");

	if(created)
		std::cerr << "result_map created\n";

	for(auto vi : result.vertices())
	{
		auto [point, normal] = APSS(result.point(vi), kd_tree, normals);
		result.point(vi)	 = point;
		result_normal[vi]	 = normal;
	}

	return result;
}

Mesh projection(const Mesh& mesh1, const Mesh& mesh2)
{
	Mesh copy_m2 = mesh2;
	// Calculating normals
	auto [mesh2_normal_map, mesh2_created] =
		copy_m2.add_property_map<Mesh::Vertex_index, Vector>("v:normal");

	CGAL::Polygon_mesh_processing::compute_vertex_normals(copy_m2, mesh2_normal_map);

	// Kd-tree indexation
	Tree kd_tree(copy_m2.vertices_begin(), copy_m2.vertices_end(), Tree::Splitter(),
				 TreeTraitsMeshAdapter(copy_m2.points()));

	return projection(mesh1, kd_tree, mesh2_normal_map);
}

static const char USAGE[] =
	R"(Write to stdout the projection of mesh from mesh-file1 to mesh from mesh-file2.

    Usage: projection <mesh-file1> <mesh-file2>

    Options:
      -h --help      Show this screen.
      --version      Show version.
)";

int main(int argc, char const* argv[])
{
	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "v1.0");

	Mesh mesh1;
	std::string mesh_file1 = args.at("<mesh-file1>").asString();

	std::cerr << "Reading mesh file 1...\n";
	if(!read_mesh_from(mesh_file1, mesh1))
		return EXIT_FAILURE;

	Mesh mesh2;
	std::string mesh_file2 = args.at("<mesh-file2>").asString();

	std::cerr << "Reading mesh file 2...\n";
	if(!read_mesh_from(mesh_file2, mesh2))
		return EXIT_FAILURE;

	Mesh global;

	global += mesh1;
	global += translated(mesh2, Vector(2.1, 0, 0));
	global += translated(projection(mesh1, mesh2), Vector(1.05, 2.1, 0));

	mesh_draw(global);
}