// Project
#include "docopt/docopt.h"
#include "mesh/conversion.hpp"
#include "mesh/utils.hpp"
#include "mesh/viewer.hpp"

// CGAL: Surface-mesh
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
// #include <CGAL/draw_surface_mesh.h>

// // CGAL: Projection
#include <CGAL/K_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
// #include <CGAL/boost/iterator/counting_iterator.hpp>

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
	R"(cut mesh with threshold.

    Usage: match <threshold> <input-files>...

    Options:
      -h --help      Show this screen
      --version      Show version
)";

int main(int argc, char const* argv[])
{
	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "v1.0");

	double threshold = 0;
	double epsilon	 = 0;

	try
	{
		threshold = stod(args.at("<threshold>").asString());
		// epsilon	  = stod(args.at("<epsilon>").asString());
	}
	catch(std::invalid_argument& ia)
	{
		std::cerr << "Error: <threshold> and <epsilon> must be a reals numbers\n";
		return EXIT_FAILURE;
	}

	auto input_files = args.at("<input-files>").asStringList();

	if(input_files.size() != 2)
	{
		std::cerr << "[ERROR] program need to have exactly 2 input files\n";
		exit(EXIT_FAILURE);
	}

	////////// MESHES IMPORTATION

	std::vector<Mesh> meshes(input_files.size());
	std::vector<std::optional<std::string>> textures(input_files.size());

	std::cerr << "[STATUS] Importing meshes from files...\n";
	{
		for(size_t i = 0; i < input_files.size(); ++i)
		{
			Mesh_data data = load_mesh_data(input_files[i]);

			meshes[i] = to_surface_mesh<Mesh>(data);

			if(i == 0)
				set_mesh_color(meshes[i], CGAL::Color(200, 0, 0));
			else if(i == 1)
				set_mesh_color(meshes[i], CGAL::Color(0, 200, 0));
			else
				set_mesh_color(meshes[i], random_color());

			textures[i] = data.texture_path;
		}
	}

	////////// MESHES PROCESSING

	auto M1_marking_map = marking(meshes[0], meshes[1], threshold);

	auto M1_division = divide(meshes[0], M1_marking_map);

	auto M2_marking_map = marking(meshes[1], meshes[0], threshold);

	auto M2_division = divide(meshes[1], M2_marking_map);
	// auto mesh = separate(meshes[0], meshes[1], threshold);

	// meshes[0].points().
	////////// MESHES VISUALISATION

	std::cerr << "[STATUS] Allocating meshes on gpu...\n";

	int qargc			 = 1;
	const char* qargv[2] = {"surface_mesh_viewer", "\0"};

	QApplication application(qargc, const_cast<char**>(qargv));

	MeshViewer viewer;

	viewer.setWindowTitle("MeshViewer");

	viewer.show(); // Create Opengl Context

	viewer.add(to_mesh_data<Mesh>(M1_division.first, textures[0]));
	viewer.add(to_mesh_data<Mesh>(M1_division.second, textures[0]));
	viewer.add(to_mesh_data<Mesh>(M2_division.first, textures[1]));
	viewer.add(to_mesh_data<Mesh>(M2_division.second, textures[1]));
	// viewer.add(to_mesh_data<Mesh>(meshes[i + 1], textures[i + 1]));
	// viewer.add(to_mesh_data<Mesh>(graft_basis[i], textures[i]));
	// viewer.add(to_mesh_data<Mesh>(graft_selection[i], textures[i + 1]));
	// viewer.add(to_mesh_data<Mesh>(graft_basis_with_transition_projected[i], textures[i]));
	// viewer.add(to_mesh_data<Mesh>(graft_selection_transition[i], textures[i + 1]));

	return application.exec();
	// return EXIT_SUCCESS;
}