// Project
#include "docopt/docopt.h"
#include "mesh/draw.hpp"
#include "mesh/io.hpp"
#include "mesh/utils.hpp"

// CGAL: Surface-mesh
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>

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

static const char USAGE[] =
	R"(Create a new mesh by matching parts of multiple similars meshes.

    Usage: match <threshold> <input-files>...

    Options:
      -h --help      Show this screen.
      --version      Show version.
)";

// x: red
// y: green
// z: blue
// mesh stitching
// remove center
// make hole
// remove_face

// TODO: METTRE LE BON KERNEL dans mesh_utils

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

int main(int argc, char const* argv[])
{
	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "v1.0");

	double threshold = 0;

	try
	{
		threshold = stod(args.at("<threshold>").asString());
	}
	catch(std::invalid_argument& ia)
	{
		std::cerr << "Error: <threshold> must be a real number\n";
		return EXIT_FAILURE;
	}

	auto input_files = args.at("<input-files>").asStringList();
	double epsilon	 = 0.005;

	Mesh global_mesh;

	std::cerr << "Reading initial mesh file...\n";
	if(!read_mesh_from(input_files.front(), global_mesh))
		return EXIT_FAILURE;

	std::cerr << "Adding global_mesh color property...\n";
	auto current_mesh_color = CGAL::Color(0, 0, 255);
	set_mesh_color(global_mesh, current_mesh_color);

	Mesh visualisation = global_mesh;

	std::cerr << "Filtering with threshold = " << threshold << '\n';
	for(size_t i = 1; i < input_files.size(); ++i)
	{
		Mesh current_mesh = global_mesh;
		// set_mesh_color(current_mesh, current_mesh_color);

		Mesh next_mesh;

		std::string next_mesh_file = input_files[i];

		std::cerr << "Reading next mesh file...\n";
		if(!read_mesh_from(next_mesh_file, next_mesh))
			return EXIT_FAILURE;

		std::cerr << "Adding next_mesh color property...\n";
		auto next_mesh_color = random_color();
		set_mesh_color(next_mesh, next_mesh_color);
		visualisation += translated(next_mesh, Vector(i * 2.1, 0, 0));

		std::cerr << "Removing points in Mesh" << i << " that are too far from Mesh" << i + 1
				  << '\n';
		{
			std::cerr << "Total vertices: " << current_mesh.number_of_vertices() << '\n';

			auto not_transition_region =
				band_stop_filter_dist(current_mesh, next_mesh, epsilon, threshold);

			Mesh transition = current_mesh;
			filter_out(transition, not_transition_region);
			set_mesh_color(transition, color_function(current_mesh_color, next_mesh_color));
			visualisation += translated(transition, Vector(-1.05 + i * 2.1, -4.2, 0));

			auto high_dist_vertices = high_pass_filter_dist(current_mesh, next_mesh, epsilon);

			filter_out(current_mesh, high_dist_vertices);
			// set_mesh_vcolor(transition, nm_vcolor, CGAL::Color(0, 255, 0));

			// current_mesh += transition;
			// print_state(current_mesh);
		}

		std::cerr << "Removing points from Mesh" << i + 1 << " that are too close from Mesh" << i
				  << '\n';
		{
			std::cerr << "Total vertices: " << next_mesh.number_of_vertices() << '\n';

			auto not_transition_region =
				band_stop_filter_dist(next_mesh, global_mesh, epsilon, threshold);

			Mesh transition = next_mesh;
			filter_out(transition, not_transition_region);
			set_mesh_color(transition, color_function(current_mesh_color, next_mesh_color));
			visualisation += translated(transition, Vector(-1.05 + i * 2.1, -4.2, 0));

			auto low_dist_vertices = low_pass_filter_dist(next_mesh, global_mesh, threshold);
			filter_out(next_mesh, low_dist_vertices);

			// next_mesh += transition;
			// print_state(next_mesh);
		}

		current_mesh_color = next_mesh_color;
		// current_mesh_file = next_mesh_file;
		global_mesh = current_mesh;
		global_mesh += next_mesh;

		visualisation += translated(global_mesh, Vector(-1.05 + i * 2.1, -2.1, 0));
	}

	std::cerr << "Displaying mesh...\n";
	// CGAL::draw(global_mesh);
	mesh_draw(visualisation);
}