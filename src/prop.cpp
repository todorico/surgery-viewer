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
	R"(List geometrical properties from mesh.

    Usage: prop (<input-file> | --from=<format>)

    Options:
      --from=<format> Input file <format> expected from stdin.
      --list=<T:P>    List elements of type T with property P
      -h --help       Show this screen.
      --version       Show version.
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

	std::cerr << "Printing mesh properties...\n";

	// list

	for(auto p : mesh.properties<Mesh::Vertex_index>())
		std::cout << p << '\n';

	for(auto p : mesh.properties<Mesh::Edge_index>())
		std::cout << p << '\n';

	for(auto p : mesh.properties<Mesh::Halfedge_index>())
		std::cout << p << '\n';

	for(auto p : mesh.properties<Mesh::Face_index>())
		std::cout << p << '\n';

	// v:color

	// for(auto v : mesh.vertices())
	// {
	// 	Mesh::Property_map<Mesh::Vertex_index, CGAL::Color> color;
	// 	bool exist = false;

	// 	std::tie(color, exist) = mesh.property_map<Mesh::Vertex_index, CGAL::Color>("v:color");
	// 	if(exist)
	// 		std::cerr << color[v] << '\n';
	// }

	// Mesh::Face_range face_range = mesh.faces;

	// for(auto face : mesh.faces())
	// {
	// 	face.
	// }

	return EXIT_SUCCESS;
}