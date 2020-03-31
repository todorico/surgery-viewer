// Project
#include "docopt/docopt.h"
#include "mesh/io.hpp"

// CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

// STD
#include <iostream>

using Kernel = CGAL::Simple_cartesian<double>;
// using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
// using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;

using Point = Kernel::Point_3;
using Mesh	= CGAL::Surface_mesh<Point>;

static const char USAGE[] =
	R"(Convert between geometrical file formats (OFF, PLY, OBJ).

    Usage: geoconv <input-file> (<output-file> | --to=<format>)

    Options:
      --to=<format>  Output <format> conversion to stdout.
      -h --help      Show this screen.
      --version      Show version.
)";

int main(int argc, char const* argv[])
{
	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "v1.0");

	Mesh mesh;
	std::string input_file = args.at("<input-file>").asString();

	std::cerr << "Reading mesh from " << input_file << "...\n";
	read_mesh_from(input_file, mesh);

	auto output_file_value = args.at("<output-file>");

	if(output_file_value)
	{
		std::string output_file = output_file_value.asString();

		std::cerr << "Writing mesh to " << output_file << "...\n";
		if(write_mesh_to(output_file, mesh))
			return EXIT_SUCCESS;
	}
	else
	{
		std::string format = args.at("--to").asString();

		std::cerr << "Writing mesh to stdout...\n";
		if(write_mesh_to(std::cout, mesh, format))
			return EXIT_SUCCESS;
	}
}