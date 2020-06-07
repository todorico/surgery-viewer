// Project
#include "mesh/view.hpp"
#include "docopt/docopt.h"
#include "mesh/conversion.hpp"
#include "mesh/data.hpp"

// #include "mesh/io.hpp"

// // CGAL
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
	R"(3D viewer for geometrical file formats (OFF, PLY, OBJ, ...).

    Usage: view <input-files>...

    Options:
      --from=<format>  Input file <format> expected from stdin.
      -h --help        Show this screen.
      --version        Show version.
)";

int main(int argc, char** argv)
{
	// COMMAND PARSING

	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "1.0");

	auto input_files = args.at("<input-files>").asStringList();

	// MESH VISUALISATION

	QApplication application(argc, argv);

	MeshViewer viewer;

	viewer.setWindowTitle("surgery-viewer");

	viewer.show(); // Create Opengl context

	// Mesh_data converted = to_mesh_data<Mesh, Kernel>(mesh);
	// converted.texture_path.emplace(data.texture_path.value());

	std::cerr << "[DEBUG] Loading meshes...\n";

	for(auto file : input_files)
	{
		viewer.add(load_mesh_data(file));
	}

	std::cerr << "[DEBUG] Mesh(es) loaded successfuly !\n";

	// CGAL::draw(mesh);
	return application.exec();
}