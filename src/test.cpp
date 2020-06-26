// STD
#include <iostream>

// CGAL
#include <CGAL/Polygon_mesh_processing/merge_border_vertices.h>

// PROJECT
#include "docopt/docopt.h"
#include "mesh/conversion.hpp"
#include "mesh/utils.hpp"
#include "mesh/viewer.hpp"

static const char USAGE[] =
	R"(cut mesh with threshold.

    Usage: test [options] <threshold> <input-files>...

    Options:
      -c, --colorize                             Colorize geometrical objects by files.
      -t <intensity>, --translation=<intensity>  Translation intensity.
      -h --help                                  Show this screen
      --version                                  Show version
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
		std::cerr << "[ERROR] <threshold> must be a real number\n";
		exit(EXIT_FAILURE);
	}

	auto input_files = args.at("<input-files>").asStringList();

	if(input_files.size() != 2)
	{
		std::cerr << "[ERROR] program needs to have exactly 2 input files\n";
		exit(EXIT_FAILURE);
	}

	////////// OPTIONS PARSING

	auto opt_colorize = args.at("--colorize").asBool();

	double translation_intensity   = 0.1;
	auto opt_translation_intensity = args.at("--translation");

	if(opt_translation_intensity)
	{
		try
		{
			translation_intensity = stod(opt_translation_intensity.asString());
		}
		catch(std::invalid_argument& ia)
		{
			std::cerr << "[ERROR] --translation=<intensity> must be a real number\n";
			exit(EXIT_FAILURE);
		}
	}

	////////// MESHES IMPORTATION

	std::vector<Surface_mesh> meshes(input_files.size());
	std::vector<std::optional<std::string>> textures(input_files.size());

	std::cerr << "[STATUS] importing meshes from files...\n";

	for(size_t i = 0; i < input_files.size(); ++i)
	{
		Mesh_data data = load_mesh_data(input_files[i]);

		meshes[i] = to_surface_mesh(data);

		// WARNING: without stictching duplicates, processing of halfedges may break.
		CGAL::Polygon_mesh_processing::stitch_borders(meshes[i]);

		if(opt_colorize)
		{
			if(i == 0)
				set_mesh_color(meshes[i], CGAL::Color(200, 0, 0));
			else if(i == 1)
				set_mesh_color(meshes[i], CGAL::Color(0, 200, 0));
			else if(i == 2)
				set_mesh_color(meshes[i], CGAL::Color(0, 0, 200));
			else
				set_mesh_color(meshes[i], random_color());
		}

		textures[i] = data.texture_path;
	}

	Surface_mesh mesh_0 = meshes[0];
	Surface_mesh mesh_1 = meshes[1];

	////////// MEHES PROCESSING

	///// MARKING REGIONS

	mark_regions(meshes[0], meshes[1], threshold);
	mark_regions(meshes[1], meshes[0], threshold);

	Surface_mesh marked_0 = meshes[0];
	Surface_mesh marked_1 = meshes[1];

	set_mesh_color(marked_0, close_vertices(marked_0), CGAL::Color(200, 0, 200));
	set_mesh_color(marked_1, close_vertices(marked_1), CGAL::Color(200, 0, 200));

	///// MARKING LIMITS

	mark_limits(meshes[0]);
	mark_limits(meshes[1]);

	set_mesh_color(meshes[0], limit_vertices(meshes[0]), CGAL::Color(200, 200, 0));
	set_mesh_color(meshes[1], limit_vertices(meshes[1]), CGAL::Color(200, 200, 0));

	///// SEPARATE REGIONS

	auto M1_division = divide(meshes[0]);
	auto M2_division = divide(meshes[1]);

	////////// MESHES VISUALISATION

	std::cerr << "[STATUS] allocating meshes on gpu...\n";

	int qargc			 = 1;
	const char* qargv[2] = {"surface_mesh_viewer", "\0"};

	QApplication application(qargc, const_cast<char**>(qargv));

	MeshViewer viewer;

	viewer.setWindowTitle("mesh_viewer");

	viewer.show(); // Create Opengl Context

	viewer.add(to_mesh_data(mesh_0, textures[0]));
	viewer.add(to_mesh_data(mesh_1, textures[1]));

	viewer.add(to_mesh_data(marked_0, textures[0]));
	viewer.add(to_mesh_data(marked_1, textures[1]));

	viewer.add(to_mesh_data(M1_division.first, textures[0]));
	viewer.add(to_mesh_data(M1_division.second, textures[0]));

	viewer.add(to_mesh_data(M2_division.first, textures[1]));
	viewer.add(to_mesh_data(M2_division.second, textures[1]));

	viewer.add(
		to_mesh_data(translated(M1_division.second, Kernel::Vector_3(0, 0, -translation_intensity)),
					 textures[0]));
	viewer.add(
		to_mesh_data(translated(M2_division.second, Kernel::Vector_3(0, 0, -translation_intensity)),
					 textures[1]));

	return application.exec();
}