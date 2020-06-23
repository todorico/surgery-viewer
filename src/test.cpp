// STD
#include <iostream>

// Project
#include "docopt/docopt.h"
#include "mesh/conversion.hpp"
#include "mesh/utils.hpp"
#include "mesh/viewer.hpp"

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

	////////// ARGUMENTS PARSING

	double threshold = 0;

	try
	{
		threshold = stod(args.at("<threshold>").asString());
		// epsilon	  = stod(args.at("<epsilon>").asString());
	}
	catch(std::invalid_argument& ia)
	{
		std::cerr << "[ERROR] <threshold> must be a reals numbers\n";
		return EXIT_FAILURE;
	}

	auto input_files = args.at("<input-files>").asStringList();

	if(input_files.size() != 2)
	{
		std::cerr << "[ERROR] program needs to have exactly 2 input files\n";
		exit(EXIT_FAILURE);
	}

	////////// MESHES IMPORTATION

	std::vector<Surface_mesh> meshes(input_files.size());
	std::vector<std::optional<std::string>> textures(input_files.size());

	std::cerr << "[STATUS] importing meshes from files...\n";

	for(size_t i = 0; i < input_files.size(); ++i)
	{
		Mesh_data data = load_mesh_data(input_files[i]);

		meshes[i] = to_surface_mesh(data);

		if(i == 0)
			set_mesh_color(meshes[i], CGAL::Color(200, 0, 0));
		else if(i == 1)
			set_mesh_color(meshes[i], CGAL::Color(0, 200, 0));
		else
			set_mesh_color(meshes[i], random_color());

		textures[i] = data.texture_path;
	}

	////////// MEHES PROCESSING

	auto M1_marking_map = mark_delimited_regions(meshes[0], meshes[1], threshold);

	auto M1_division = divide(meshes[0], M1_marking_map);

	auto M2_marking_map = mark_delimited_regions(meshes[1], meshes[0], threshold);

	auto M2_division = divide(meshes[1], M2_marking_map);

	////////// MESHES VISUALISATION

	std::cerr << "[STATUS] allocating meshes on gpu...\n";

	int qargc			 = 1;
	const char* qargv[2] = {"surface_mesh_viewer", "\0"};

	QApplication application(qargc, const_cast<char**>(qargv));

	MeshViewer viewer;

	viewer.setWindowTitle("mesh_viewer");

	viewer.show(); // Create Opengl Context

	viewer.add(to_mesh_data(M1_division.first, textures[0]));
	viewer.add(to_mesh_data(M1_division.second, textures[0]));
	viewer.add(to_mesh_data(M2_division.first, textures[1]));
	viewer.add(to_mesh_data(M2_division.second, textures[1]));

	return application.exec();
}