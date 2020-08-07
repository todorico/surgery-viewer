// STD
#include <iostream>

// PROJECT
#include "docopt/docopt.h"
#include "mesh/conversion.hpp"
#include "mesh/export.hpp"
#include "mesh/import.hpp"
#include "mesh/marking.hpp"
#include "mesh/projection.hpp"
#include "mesh/utils.hpp"
#include "mesh/viewer.hpp"

// CGAL
#include <CGAL/Polygon_mesh_processing/merge_border_vertices.h>
// #include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <boost/range/join.hpp>

// Associe un triangle à une annotation (close/distant/limit)
template <class VertexRange>
Vertex_mark triangle_mark(const VertexRange& triangle_vertices,
                          const SM_marking_map& marking_map)
{
    for(auto v : triangle_vertices)
    {
        if(marking_map[v] == Vertex_mark::Close)
        {
            return Vertex_mark::Close;
        }
        else if(marking_map[v] == Vertex_mark::Distant)
        {
            return Vertex_mark::Distant;
        }
    }

    return Vertex_mark::Limit;
}

// Vérifie si le point 'p' se projette sur les sommets de 'triangle_vertices'
template <class VertexRange>
bool is_point_projected_on_triangle(const Surface_mesh& mesh,
                                    const VertexRange& triangle_vertices,
                                    const Kernel::Point_3& p)
{
    if(triangle_vertices.size() != 3)
    {
        std::cerr
            << "[ERROR] triangle_vertices must be of size 3 but is of size "
            << triangle_vertices.size() << '\n';
        exit(EXIT_FAILURE);
    }

    // Get triangle points

    auto v_it = triangle_vertices.begin();

    auto a = mesh.point(*v_it);
    ++v_it;
    auto b = mesh.point(*v_it);
    ++v_it;
    auto c = mesh.point(*v_it);

    // Create triangle

    Kernel::Triangle_3 triangle(a, b, c);

    // Create triangle perpendicular line

    auto perpendicular_line = triangle.supporting_plane().perpendicular_line(p);

    return CGAL::do_intersect(perpendicular_line, triangle);
}

// Cette algorithme copy les annotation de M2 sur M1
// Precondition 1 : M1 doit d'abord etre projeté sur M2
// Precondition 2 : M2 doit avoir un carte d'annotation associé (SM_marking_map)
SM_marking_map mark_regions(Surface_mesh& M1, const Surface_mesh& M2,
                            const SM_kd_tree& M2_tree)
{
    auto [M1_marking_map, created] =
        M1.add_property_map<Surface_mesh::Vertex_index, Vertex_mark>(
            "v:mark", Vertex_mark::None);

    auto M2_marking_map = get_marking_map(M2);

    for(auto M1_v : M1.vertices())
    {
        auto M1_point = M1.point(M1_v);

        SM_kd_tree_search search(M2_tree, M1_point, 1, 0, true,
                                 M2_tree.traits().point_property_map());

        auto [M2_v, M2_dist_squared1] = *(search.begin());

        auto faces = CGAL::halfedges_around_target(M2_v, M2);

        for(auto ff : faces)
        {
            auto faces_around_face = CGAL::halfedges_around_target(ff, M2);

            for(auto f : faces_around_face)
            {
                if(M2.is_border(f))
                {
                    // std::cerr << "[WARNING] skipping border halfedge\n";
                    continue;
                }

                auto M2_triangle_vertices = CGAL::vertices_around_face(f, M2);

                if(is_point_projected_on_triangle(M2, M2_triangle_vertices,
                                                  M1_point))
                {
                    M1_marking_map[M1_v] =
                        triangle_mark(M2_triangle_vertices, M2_marking_map);
                    break;
                }
            }
        }
    }

    return M1_marking_map;
}

SM_marking_map mark_regions(Surface_mesh& M1, const Surface_mesh& M2)
{
    SM_kd_tree M2_tree(M2.vertices().begin(), M2.vertices().end(),
                       SM_kd_tree_splitter(),
                       SM_kd_tree_traits_adapter(M2.points()));

    return mark_regions(M1, M2, M2_tree);
}

SM_marking_map mark_delimited_regions(Surface_mesh& M1, const Surface_mesh& M2)
{
    mark_regions(M1, M2);
    return mark_limits(M1);
}

struct Scene_data
{
    std::unique_ptr<aiScene> scene;
    unsigned int mesh_index;
    aiMesh* mesh;
    unsigned int material_index;
    aiMaterial* material;
    std::string material_path;
    std::string extension;
};

Scene_data import_scene_data(const std::string& filename)
{
    //  Importing scene data from file
    auto scene = import_scene(filename);

    // Check scene status
    print_scene_status(scene.get());

    // Finding mesh data from scene
    unsigned int mesh_index = find_mesh_index(scene.get());
    aiMesh* mesh            = scene->mMeshes[mesh_index];

    // Finding mesh material index
    unsigned int material_index = mesh->mMaterialIndex;
    aiMaterial* material        = scene->mMaterials[material_index];

    // Finding texture path from material
    std::string textures_path = find_texture_path(filename, material);

    // Finding filename extension
    auto n = filename.find_last_of(".");

    std::string extension;

    // did not found pattern
    if(n == std::string::npos)
    {
        extension = "obj"; // default extension
    }
    else
    {
        // extension = filename.substr(n, filename.size());
        extension = "obj"; // force obj output
    }

    return {std::move(scene), mesh_index,    mesh,     material_index,
            material,         textures_path, extension};
}

void update_scene_mesh_data(Scene_data& scene_data, const Surface_mesh& mesh)
{
    delete scene_data.scene->mMeshes[scene_data.mesh_index];

    scene_data.scene->mMeshes[scene_data.mesh_index] =
        make_ai_mesh(mesh).release();
    scene_data.scene->mMeshes[scene_data.mesh_index]->mMaterialIndex =
        scene_data.material_index;
}

// export scene by modifying scene_data
aiReturn export_scene_data(Scene_data& scene_data, const std::string& filename)
{
    return export_scene(scene_data.extension, filename, scene_data.scene.get());
}

// Précondition : (dist_close >= 0 && dist_distant >= 0)
double reprojection_coeff(double dist_close, double dist_distant)
{
    if(dist_close == 0 || (dist_close + dist_distant) == 0)
        return 1;

    return ((dist_close * dist_distant) / dist_close) /
           (dist_close + dist_distant);
}

// Reprojette certains sommet en fonction de leurs distances par rapport au
// point proches et distants du maillages
template <class VertexRange>
Surface_mesh reprojection(const Surface_mesh& M1, const VertexRange& M1_vertices,
                  const Surface_mesh& M1_proj, const SM_kd_tree& close_tree,
                  const SM_kd_tree& distant_tree)
{
    Surface_mesh result(M1);

    for(auto M1_v : M1_vertices)
    {
        auto M1_point = result.point(M1_v);

        SM_kd_tree_search search_close(
            close_tree, M1_point, 1, 0, true,
            close_tree.traits().point_property_map());

        SM_kd_tree_search search_distant(
            distant_tree, M1_point, 1, 0, true,
            distant_tree.traits().point_property_map());

        auto [close_v, close_dist_squared]     = *(search_close.begin());
        auto [distant_v, distant_dist_squared] = *(search_distant.begin());

        double k = reprojection_coeff(std::sqrt(close_dist_squared),
                                      std::sqrt(distant_dist_squared));

        auto M1_proj_point = M1_proj.point(M1_v);

        Kernel::Vector_3 v = (M1_proj_point - M1_point) * k;

        result.point(M1_v) = (M1_point + v);
    }

    return result;
}

template <class VertexRange>
Surface_mesh reprojection(const Surface_mesh& M1, const VertexRange& M1_vertices,
                  const Surface_mesh& M1_proj)
{
    auto close_limit_vertices =
        marked_vertices(M1_proj, Vertex_mark::Limit, Vertex_mark::Close);
    auto distant_limit_vertices =
        marked_vertices(M1_proj, Vertex_mark::Limit, Vertex_mark::Distant);

    SM_kd_tree close_tree(close_limit_vertices.begin(),
                          close_limit_vertices.end(), SM_kd_tree_splitter(),
                          SM_kd_tree_traits_adapter(M1.points()));

    SM_kd_tree distant_tree(distant_limit_vertices.begin(),
                            distant_limit_vertices.end(), SM_kd_tree_splitter(),
                            SM_kd_tree_traits_adapter(M1.points()));

    return reprojection(M1, M1_vertices, M1_proj, close_tree, distant_tree);
}

Surface_mesh reproject_transition(const Surface_mesh& M1, const Surface_mesh& M1_proj)
{
    return reprojection(M1, limit_vertices(M1_proj), M1_proj);
}

static const char USAGE[] =
    R"(Create a new mesh by matching parts of multiple similars meshes.

    Usage: match [options] <threshold> <input-files>...

    Options:
      -c, --colorize                       Colorize geometrical objects by files.
      -e <offset>, --epsilon <offest>	   Augment threshold to make transition regions.
      -a, --export-all                     Export all meshes components
      -h --help                            Show this screen
      --version                            Show version
)";

int main(int argc, char const* argv[])
{
    std::map<std::string, docopt::value> args =
        docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "v1.0");

    ////// PROGRAM ARGUMENTS

    double threshold = 0;
    // double reproject_offest = thres

    try
    {
        threshold = std::stod(args.at("<threshold>").asString());
    }
    catch(std::invalid_argument& ia)
    {
        std::cerr << "[ERROR] <threshold> must be a reals numbers\n";
        exit(EXIT_FAILURE);
    }

    auto input_files = args.at("<input-files>").asStringList();

    ////// PROGRAM OPTIONS

    auto opt_colorize   = args.at("--colorize").asBool();
    auto opt_export_all = args.at("--export-all").asBool();

    double epsilon   = threshold;
    auto opt_epsilon = args.at("--epsilon");

    if(opt_epsilon)
    {
        try
        {
            epsilon = std::stod(opt_epsilon.asString());
        }
        catch(std::invalid_argument& ia)
        {
            std::cerr << "[ERROR] --epsilon=<dist> must be a real number\n";
            exit(EXIT_FAILURE);
        }
    }

    ////////// ASSIMP DATA IMPORTATION

    auto glob_scene_data   = import_scene_data(input_files.front());
    Surface_mesh glob_mesh = make_surface_mesh(glob_scene_data.mesh);

    // WARNING: force mesh to be geometricaly processable by removing duplicated
    // halfedges
    CGAL::Polygon_mesh_processing::stitch_borders(glob_mesh);

    if(opt_colorize)
        set_mesh_color(glob_mesh, {1.0f, 0.0f, 0.0f, 1.0f});

    ////////// MESH PROCESSING

    for(size_t i = 1; i < input_files.size(); ++i)
    {
        ////////// ASSIMP DATA IMPORTATION

        auto next_scene_data   = import_scene_data(input_files[i]);
        Surface_mesh next_mesh = make_surface_mesh(next_scene_data.mesh);

        CGAL::Polygon_mesh_processing::stitch_borders(next_mesh);

        Surface_mesh curr_mesh = glob_mesh;

        ////////// MESHES STATISTICS

        std::cerr << "[CURR_MESH] total vertices: "
                  << curr_mesh.number_of_vertices() << '\n';

        std::cerr << "[NEXT_MESH] total vertices: "
                  << next_mesh.number_of_vertices() << '\n';

        ////////// FULL COLORIZATION

        if(opt_colorize)
        {
            if(i == 1)
                set_mesh_color(next_mesh, {0.0f, 1.0f, 0.0f, 1.0f});
            else if(i == 2)
                set_mesh_color(next_mesh, {0.0f, 0.0f, 1.0f, 1.0f});
            else
                set_mesh_color(next_mesh, random_color());
        }

        std::cerr << "[NEXT_MESH] Projecting...\n";
        Surface_mesh next_proj = projection(next_mesh, curr_mesh);

        ////////// MARKING

        std::cerr << "[CURR_MESH] Marking...\n";
        mark_delimited_regions(curr_mesh, next_mesh, threshold, epsilon);

        std::cerr << "[NEXT_MESH_PROJECTED] Marking...\n";
        mark_delimited_regions(next_proj, curr_mesh);

        std::cerr << "[NEXT_MESH] Partial reprojection...\n";
        next_mesh = reproject_transition(next_mesh, next_proj); // adapte la géométrie de next pour s'adapter à curr

        ////////// LIMITS COLORIZATION

        if(opt_colorize)
        {
            // Afficher sommets transitions/limit en jaune
            set_mesh_color(curr_mesh, limit_vertices(curr_mesh),
                           {1.0f, 1.0f, 0.0f, 1.0f});
            set_mesh_color(next_proj, limit_vertices(next_proj),
                           {1.0f, 1.0f, 0.0f, 1.0f});

            // // Afficher sommets proche/limit en violet
            // set_mesh_color(next_proj, next_close_limit_vertices,
            //                {1.0f, 0.0f, 1.0f, 1.0f});

            // // Afficher sommets distant/limit en vert/bleu
            // set_mesh_color(next_proj, next_distant_limit_vertices,
            //                {0.0f, 1.0f, 1.0f, 1.0f});
        }

        ////////// DIVISION

        // Remove transition mark on curr_mesh
        mark_limits_with(curr_mesh, Vertex_mark::Distant);
        // Set limits on curr_mesh
        mark_limits(curr_mesh);

        auto [curr_close, curr_distant] = divide(curr_mesh);

        auto [next_proj_close, next_proj_distant] =
            divide(std::move(next_proj));

        std::cerr << "[NEXT_MESH] Filtering...\n";

        Surface_mesh next_close =
            filtered(next_mesh, distant_vertices(next_proj));
        Surface_mesh next_distant =
            filtered(next_mesh, close_vertices(next_proj));

        ////////// RECONSTRUCTION

        // [WARNING] decommentez ces lignes pour pouvoir executez l'algorithme
        // sur plus de 2 maillages. Cependant il faudra d'abor boucher les
        // trous.

        // glob_mesh = curr_close;
        // glob_mesh += next_distant;

        // update_scene_mesh_data(glob_scene_data, glob_mesh);
        // export_scene_data(glob_scene_data,
        //                   "M" + std::to_string(i) +
        //                   "_reconstruction.obj");

        ////////// EXPORT MESHES

        std::cerr << "[STATUS] exporting...\n";

        // Elements permettant la reconstruction des étapes

        update_scene_mesh_data(glob_scene_data, curr_close);
        export_scene_data(glob_scene_data,
                          "M" + std::to_string(i - 1) + "_close.obj");

        update_scene_mesh_data(glob_scene_data, curr_distant);
        export_scene_data(glob_scene_data,
                          "M" + std::to_string(i - 1) + "_distant.obj");

        update_scene_mesh_data(next_scene_data, next_distant);
        export_scene_data(next_scene_data,
                          "M" + std::to_string(i) + "_distant.obj");

        // Elements intermediares

        if (opt_export_all)
        {
            update_scene_mesh_data(next_scene_data, next_close);
            export_scene_data(next_scene_data,
                            "M" + std::to_string(i) + "_close.obj");

            update_scene_mesh_data(next_scene_data, next_proj_close);
            export_scene_data(next_scene_data,
                            "M" + std::to_string(i) + "_proj_close.obj");

            update_scene_mesh_data(next_scene_data, next_proj_distant);
            export_scene_data(next_scene_data,
                            "M" + std::to_string(i) + "_proj_distant.obj");
        }
    }

    return EXIT_SUCCESS;
}