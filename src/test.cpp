// STD
#include <iostream>

// CGAL
#include <CGAL/Polygon_mesh_processing/merge_border_vertices.h>

// PROJECT
#include "docopt/docopt.h"
#include "mesh/conversion.hpp"
#include "mesh/export.hpp"
#include "mesh/import.hpp"
// #include "mesh/projection.hpp"
// #include "mesh/utils.hpp"
// #include "mesh/viewer.hpp"

static const char USAGE[] =
	R"(test programs for experimenting.

    Usage: test [options] <input-files>...

    Options:
      -c, --colorize                             Colorize geometrical objects by files.
      -e <dist>, --epsilon <dist>				 Reduce threshold to make transition.
      -t <intensity>, --translation=<intensity>  Translation intensity.
      -h --help                                  Show this screen
      --version                                  Show version
)";

int main(int argc, char const* argv[])
{
	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "v1.0");

	////////// ARGUMENTS PARSING

	auto input_files = args.at("<input-files>").asStringList();

	////////// MESHES IMPORTATION

	std::vector<Surface_mesh> meshes(input_files.size());
	std::vector<std::optional<std::string>> textures(input_files.size());

	std::cerr << "[STATUS] importing meshes from files...\n";

	for(size_t i = 0; i < input_files.size(); ++i)
	{
		// const aiScene* scene = import(input_files[i]);

		//  Importing data from file
		auto scene = import_scene(input_files[i]);

		print_scene_status(scene.get());

		auto mesh_data_index = find_mesh_index(scene.get());
		auto mesh_data		 = scene->mMeshes[mesh_data_index];
		auto mesh_material	 = scene->mMaterials[mesh_data->mMaterialIndex];
		auto mesh_texture_path =
			find_texture_path(input_files[i], mesh_material);

		// Creating surface mesh
		auto surface_mesh = make_surface_mesh(mesh_data);

		auto new_mesh_data			  = make_ai_mesh(surface_mesh);
		new_mesh_data->mMaterialIndex = mesh_data->mMaterialIndex;

		assign_scene_mesh(scene.get(), mesh_data_index,
						  new_mesh_data.release());

		export_scene("obj", "test.obj", scene.get());
	}

	return 0;
}