// Project
#include "docopt/docopt.h"
#include "mesh/draw.hpp"
#include "mesh/io.hpp"

// CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>

// STD
#include <iostream>

using Kernel = CGAL::Simple_cartesian<double>;
// using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
// using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;

using Point = Kernel::Point_3;
using Mesh	= CGAL::Surface_mesh<Point>;

static const char USAGE[] =
	R"(3D viewer for geometrical file formats (OFF, PLY, OBJ).

    Usage: view (<input-file> | --from=<format>)

    Options:
      --from=<format>  Input file <format> expected from stdin.
      -h --help        Show this screen.
      --version        Show version.
)";

int main(int argc, char const* argv[])
{
	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "1.0");

	Mesh mesh;
	auto input_file_value = args.at("<input-file>");

	if(input_file_value)
	{
		std::string input_file = input_file_value.asString();

		std::cerr << "Reading mesh from " << input_file << "...\n";
		if(!read_mesh_from(input_file, mesh))
			return EXIT_FAILURE;
	}
	else
	{
		std::string format = args.at("--from").asString();

		std::cerr << "Reading mesh from stdin...\n";
		if(!read_mesh_from(std::cin, mesh, format))
			return EXIT_FAILURE;
	}

	if(mesh.is_empty())
	{
		std::cerr << "Error: mesh is empty\n";
		return EXIT_FAILURE;
	}

	std::cerr << "Displaying mesh...\n";
	mesh_draw(mesh);
	// CGAL::draw(mesh);

	return EXIT_SUCCESS;
}