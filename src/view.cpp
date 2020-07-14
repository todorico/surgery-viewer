// STD
#include <iostream>
#include <random>

// PROJECT
#include "docopt/docopt.h"
#include "mesh/viewer.hpp"

static const char USAGE[] =
	R"(3D viewer for geometrical file formats (OFF, PLY, OBJ, ...).

    Usage: view [options] <input-files>...

    Options:
      -c, --colorize  Colorize geometrical objects by files.
      -h, --help      Show this screen.
      --version      Show version.
)";

int main(int argc, char** argv)
{
	// ARGUMENTS PARSING

	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "1.0");

	auto input_files = args.at("<input-files>").asStringList();

	auto colorize = args.at("--colorize").asBool();

	// VISUALISATION

	QApplication application(argc, argv);
	
	//QSurfaceFormat glFormat;
 	//glFormat.setVersion(3, 0);
 	//glFormat.setProfile(QSurfaceFormat::CompatibilityProfile); // Requires >=Qt-4.8.0
	
	MeshViewer viewer;

	//viewer.setFormat(glFormat);

	viewer.setWindowTitle("surgery-viewer");

	viewer.show(); // Create Opengl context

	std::cerr << "[DEBUG] Loading meshes...\n";

	for(size_t i = 0; i < input_files.size(); ++i)
	{
		Mesh_data data = load_mesh_data(input_files[i]);

		if(colorize)
		{
			if(i == 0)
			{
				data.colors.emplace(std::vector<Mesh_data::vec_4f>(data.positions->size(),
																   {0.9f, 0.0f, 0.0f, 1.0f}));
			}
			else if(i == 1)
			{
				data.colors.emplace(std::vector<Mesh_data::vec_4f>(data.positions->size(),
																   {0.0f, 0.9f, 0.0f, 1.0f}));
			}
			else if(i == 2)
			{
				data.colors.emplace(std::vector<Mesh_data::vec_4f>(data.positions->size(),
																   {0.0f, 0.0f, 0.9f, 1.0f}));
			}
		}

		viewer.add(data);
	}

	std::cerr << "[DEBUG] Mesh(es) loaded successfuly !\n";

	return application.exec();
}