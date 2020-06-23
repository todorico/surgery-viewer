// STD
#include <iostream>

// PROJECT
#include "docopt/docopt.h"
#include "mesh/conversion.hpp"

static const char USAGE[] =
	R"(List geometrical properties from mesh.

    Usage: prop <input-file>

    Options:
      -h --help       Show this screen.
      --version       Show version.
)";

int main(int argc, char const* argv[])
{
	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "1.0");

	std::string filename = args.at("<input-file>").asString();

	std::clog << "[STATUS] reading data from " << filename << "...\n";
	Mesh_data data = load_mesh_data(filename);

	std::clog << "[STATUS] converting data to surface mesh...\n";
	Surface_mesh mesh = to_surface_mesh(data);

	if(mesh.is_empty())
	{
		std::cerr << "[ERROR] surface mesh is empty\n";
		exit(EXIT_FAILURE);
	}

	std::clog << "[STATUS] printin surface mesh properties...\n";

	for(auto p : mesh.properties<Surface_mesh::Vertex_index>())
		std::cout << p << '\n';

	for(auto p : mesh.properties<Surface_mesh::Edge_index>())
		std::cout << p << '\n';

	for(auto p : mesh.properties<Surface_mesh::Halfedge_index>())
		std::cout << p << '\n';

	for(auto p : mesh.properties<Surface_mesh::Face_index>())
		std::cout << p << '\n';

	// v:color

	// for(auto v : mesh.vertices())
	// {
	// 	Surface_mesh::Property_map<Surface_mesh::Vertex_index, CGAL::Color> color;
	// 	bool exist = false;

	// 	std::tie(color, exist) = mesh.property_map<Surface_mesh::Vertex_index,
	// CGAL::Color>("v:color"); 	if(exist) 		std::cerr << color[v] << '\n';
	// }

	// Surface_mesh::Face_range face_range = mesh.faces;

	// for(auto face : mesh.faces())
	// {
	// 	face.
	// }

	return EXIT_SUCCESS;
}