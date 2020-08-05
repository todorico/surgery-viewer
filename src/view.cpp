// STD
#include <iostream>
#include <random>

// PROJECT
#include "docopt/docopt.h"
#include "mesh/conversion.hpp"
#include "mesh/import.hpp"
#include "mesh/utils.hpp"
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

    // QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setVersion(3, 0);

    QSurfaceFormat::setDefaultFormat(format);

    QApplication application(argc, argv);

    MeshViewer viewer;
    viewer.setWindowTitle("surgery-viewer");
    viewer.show(); // Create Opengl context

    std::cerr << "[DEBUG] Loading meshes...\n";

    for(size_t i = 0; i < input_files.size(); ++i)
    {
        //  Importing scene data from file
        auto scene = import_scene(input_files[i]);
        print_scene_status(scene.get());

        //  Finding mesh data from scene
        auto mesh_data_index = find_mesh_index(scene.get());
        auto mesh_data       = scene->mMeshes[mesh_data_index];

        //  Finding texture data from mesh
        auto mesh_material = scene->mMaterials[mesh_data->mMaterialIndex];
        auto mesh_texture_path =
            find_texture_path(input_files[i], mesh_material);

        std::cerr << "texture_path_found" << mesh_texture_path << '\n';

        // Creating surface mesh
        auto surface_mesh = make_surface_mesh(mesh_data);

        if(colorize)
        {
            if(i == 0)
            {
                set_mesh_color(surface_mesh, {1.0f, 0.0f, 0.0f, 1.0f});
            }
            else if(i == 1)
            {
                set_mesh_color(surface_mesh, {0.0f, 1.0f, 0.0f, 1.0f});
            }
            else if(i == 2)
            {
                set_mesh_color(surface_mesh, {0.0f, 0.0f, 1.0f, 1.0f});
            }
            else
            {
                set_mesh_color(surface_mesh, random_color());
            }
        }

        viewer.add(to_mesh_data(surface_mesh, mesh_texture_path));
    }

    std::cerr << "[DEBUG] Mesh(es) loaded successfuly !\n";

    return application.exec();
}