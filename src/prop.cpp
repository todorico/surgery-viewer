// STD
#include <iostream>

// CGAL
#include <CGAL/Polygon_mesh_processing/merge_border_vertices.h>

// PROJECT
#include "docopt/docopt.h"
#include "mesh/import.hpp"

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

	//  Importing scene data from file
	auto scene = import_scene(filename);
	print_scene_status(scene.get());

	//  Importing mesh data from scene
	auto mesh_data_index = find_mesh_index(scene.get());
	auto mesh			 = make_surface_mesh(scene->mMeshes[mesh_data_index]);

	CGAL::Polygon_mesh_processing::stitch_borders(mesh);

	if(mesh.is_empty())
	{
		std::cerr << "[ERROR] surface mesh is empty\n";
		exit(EXIT_FAILURE);
	}

	std::clog << "[STATUS] printin surface mesh properties...\n";
	std::clog << "number_of_edges:     " << mesh.number_of_edges() << '\n';
	std::clog << "number_of_faces:     " << mesh.number_of_faces() << '\n';
	std::clog << "number_of_halfedges: " << mesh.number_of_halfedges() << '\n';
	std::clog << "number_of_vertices:  " << mesh.number_of_vertices() << '\n';

	for(auto p : mesh.properties<Surface_mesh::Vertex_index>())
		std::cout << p << '\n';

	for(auto p : mesh.properties<Surface_mesh::Edge_index>())
		std::cout << p << '\n';

	for(auto p : mesh.properties<Surface_mesh::Halfedge_index>())
		std::cout << p << '\n';

	for(auto p : mesh.properties<Surface_mesh::Face_index>())
		std::cout << p << '\n';

	return EXIT_SUCCESS;
}