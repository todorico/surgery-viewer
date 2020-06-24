// STD
#include <iostream>

// PROJECT
#include "docopt/docopt.h"
#include "mesh/conversion.hpp"
#include "mesh/projection.hpp"
#include "mesh/utils.hpp"
#include "mesh/viewer.hpp"

// CGAL
#include <CGAL/Polygon_mesh_processing/merge_border_vertices.h>
// #include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <boost/range/join.hpp>

template <class VertexRange>
bool is_not_close_face(const VertexRange& face_vertices, const SM_marking_map& marking_map)
{
	for(auto v : face_vertices)
	{
		if(marking_map[v] == Vertex_mark::Close)
		{
			return false;
		}
	}

	return true;
}

template <class VertexRange>
bool is_not_limit_face(const VertexRange& face_vertices, const SM_marking_map& marking_map)
{
	for(auto v : face_vertices)
	{
		if(marking_map[v] == Vertex_mark::Limit)
		{
			return false;
		}
	}

	return true;
}

template <class VertexRange>
bool is_limit_face(const VertexRange& face_vertices, const SM_marking_map& marking_map)
{
	bool found_distant = false;
	bool found_close   = false;
	bool found_limit   = false;

	for(auto v : face_vertices)
	{
		if(marking_map[v] == Vertex_mark::Close)
		{
			found_close = true;
		}
		else if(marking_map[v] == Vertex_mark::Distant)
		{
			found_distant = true;
		}
		else
		{
			found_limit = true;
		}
	}

	// TODO: CHECK CONDITION

	return (found_close && found_distant) || (!found_close && !found_distant && found_limit);
}

template <class VertexRange>
bool is_point_projected_on_triangle(const Surface_mesh& mesh, const VertexRange& triangle_vertices,
									const Kernel::Point_3& p)
{
	if(triangle_vertices.size() != 3)
	{
		std::cerr << "[ERROR] triangle_vertices must be of size 3 but is of size "
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

SM_marking_map mark_regions(Surface_mesh& M1, const Surface_mesh& M2, const SM_kd_tree& M2_tree)
{
	auto [M1_marking_map, created] =
		M1.add_property_map<Surface_mesh::Vertex_index, Vertex_mark>("v:mark", Vertex_mark::None);

	auto M2_marking_map = get_marking_map(M2);
	// std::vector<Surface_mesh::Vertex_index> test_vertices = {
	// 	Surface_mesh::Vertex_index(110), Surface_mesh::Vertex_index(220),
	// 	Surface_mesh::Vertex_index(440), Surface_mesh::Vertex_index(1200),
	// 	Surface_mesh::Vertex_index(990)};

	for(auto M1_v : M1.vertices())
	{
		auto M1_point = M1.point(M1_v);

		SM_kd_tree_search search(M2_tree, M1_point, 1, 0, true,
								 M2_tree.traits().point_property_map());

		auto [M2_v, M2_dist_squared1] = *(search.begin());

		auto faces = CGAL::halfedges_around_target(M2_v, M2);
		// auto faces = boost::join(CGAL::halfedges_around_target(M2_v, M2),
		// 						 CGAL::halfedges_around_face(M2.halfedge(M2_v), M2));

		// std::cerr << "nb faces : " << faces.size() << '\n';
		// std::cerr << "degree v : " << M2.degree(M2_v) << '\n';

		bool found_distant_triangle	 = false;
		bool found_matching_triangle = false;

		for(auto f : faces)
		{
			if(M2.is_border(f))
			{
				std::cerr << "[WARNING] skipping border halfedge\n";
				continue;
			}

			auto M2_triangle_vertices = CGAL::vertices_around_face(f, M2);
			// auto M2_triangle_vertices = CGAL::vertices_around_face(f, M1);
			// std::cerr << "nb points : " << M2_triangle_vertices.size() << '\n';
			// // std::cerr << "face size : " << M2_triangle_vertices.size() << '\n';

			found_matching_triangle =
				is_point_projected_on_triangle(M2, M2_triangle_vertices, M1_point);

			// std::cerr << "after vertices around face\n";
			if(found_matching_triangle && is_not_close_face(M2_triangle_vertices, M2_marking_map))
			{
				found_distant_triangle = true;
				break;
			}
		}

		if(!found_matching_triangle)
		{
			std::cerr << "[WARNING] could not found matching triangle for vertex " << M1_v << '\n';
		}

		if(found_distant_triangle)
			M1_marking_map[M1_v] = Vertex_mark::Distant;
		else
			M1_marking_map[M1_v] = Vertex_mark::Close;
	}

	return M1_marking_map;
}

SM_marking_map mark_regions(Surface_mesh& M1, const Surface_mesh& M2)
{
	SM_kd_tree M2_tree(M2.vertices().begin(), M2.vertices().end(), SM_kd_tree_splitter(),
					   SM_kd_tree_traits_adapter(M2.points()));

	return mark_regions(M1, M2, M2_tree);
}

SM_marking_map mark_delimited_regions(Surface_mesh& M1, const Surface_mesh& M2)
{
	auto M1_marking_map = mark_regions(M1, M2);
	return mark_limits(M1, M1_marking_map);
}

template <class VertexRange>
Surface_mesh stick_vertices(const Surface_mesh& M1, const VertexRange& M1_vertices,
							const Surface_mesh M2, const SM_kd_tree& M2_tree)
{
	Surface_mesh result = M1;
	auto M1_marking_map = get_marking_map(result);
	// auto M2_marking_map = get_marking_map(M2);

	for(auto M1_v : M1_vertices)
	{
		// auto M1_point = M1.point(M1_v);

		SM_kd_tree_search search(M2_tree, result.point(M1_v), 1, 0, true,
								 M2_tree.traits().point_property_map());

		auto [M2_v, M2_dist_squared] = *(search.begin());

		result.point(M1_v)	 = M2_tree.traits().point_property_map()[M2_v];
		M1_marking_map[M1_v] = Vertex_mark::Limit;
	}

	return result;
}

template <class VertexRange>
Surface_mesh stick_vertices(const Surface_mesh& M1, const VertexRange& M1_vertices,
							const Surface_mesh& M2, const VertexRange& M2_vertices)
{
	SM_kd_tree M2_tree(M2_vertices.begin(), M2_vertices.end(), SM_kd_tree_splitter(),
					   SM_kd_tree_traits_adapter(M2.points()));

	return stick_vertices(M1, M1_vertices, M2, M2_tree);
}

// renvoie les vertex de m2 qui ne sont pas coller aux vertex de M1

template <class VertexRange>
auto not_sticked_vertices(const Surface_mesh& M1, const VertexRange& M1_vertices,
						  const SM_kd_tree& M2_tree)
{
	std::vector<Surface_mesh::Vertex_index> result;

	std::copy_if(M1_vertices.begin(), M1_vertices.end(), std::back_inserter(result),
				 [&](auto M1_v) {
					 SM_kd_tree_search search(M2_tree, M1.point(M1_v), 1, 0, true,
											  M2_tree.traits().point_property_map());

					 auto [M2_v, M2_dist_squared] = *(search.begin());

					 return M2_dist_squared > 0;
				 });

	return result;
}

template <class VertexRange>
auto not_sticked_vertices(const Surface_mesh& M1, const VertexRange& M1_vertices,
						  const Surface_mesh& M2, const VertexRange& M2_vertices)
{
	SM_kd_tree M2_tree(M2_vertices.begin(), M2_vertices.end(), SM_kd_tree_splitter(),
					   SM_kd_tree_traits_adapter(M2.points()));

	return not_sticked_vertices(M1, M1_vertices, M2_tree);
}

Surface_mesh fill_holes(const Surface_mesh& M1, const Surface_mesh& M2)
{
	// point limites de M2 qui ne sont pas collé a M1
	auto M2_not_sticked_vertices =
		not_sticked_vertices(M2, limit_vertices(M2), M1, distant_vertices(M1));

	return stick_vertices(M1, distant_vertices(M1), M2, M2_not_sticked_vertices);
}

static const char USAGE[] =
	R"(Create a new mesh by matching parts of multiple similars meshes.

    Usage: match [options] <threshold> <input-files>...

    Options:
      -h --help           Show this screen
      --neighbors=<num>   Numbers of neighbors to query during projection
      --iterations=<num>  Numbers of iterations during projection
      --version           Show version
)";

int main(int argc, char const* argv[])
{
	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "v1.0");

	////// PROGRAM ARGUMENTS

	double threshold = 0;

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

	unsigned long iterations = 20;
	auto opt_iterations		 = args.at("--iterations");

	if(opt_iterations)
	{
		try
		{
			iterations = std::stoul(opt_iterations.asString());
		}
		catch(std::invalid_argument& ia)
		{
			std::cerr << "[ERROR] --iterations=<nb> must be an unsigned integer\n";
			exit(EXIT_FAILURE);
		}
	}

	unsigned long neighbors = 20;
	auto opt_neighbors		= args.at("--neighbors");

	if(opt_neighbors)
	{
		try
		{
			neighbors = std::stoul(opt_neighbors.asString());
		}
		catch(std::invalid_argument& ia)
		{
			std::cerr << "[ERROR] --neighbors=<nb> must be an unsigned integer\n";
			exit(EXIT_FAILURE);
		}
	}

	////////// MESH IMPORTATION

	std::vector<Surface_mesh> meshes(input_files.size());
	std::vector<std::optional<std::string>> textures(input_files.size());

	std::cerr << "[STATUS] Importing meshes from files...\n";
	{
		for(size_t i = 0; i < input_files.size(); ++i)
		{
			Mesh_data data = load_mesh_data(input_files[i]);

			meshes[i] = to_surface_mesh(data);
			// CGAL::Polygon_mesh_processing::merge_duplicated_vertices_in_boundary_cycles(meshes[i]);

			// WARNING: force mesh to be geometricaly processable by removing duplicated halfedges
			CGAL::Polygon_mesh_processing::stitch_borders(meshes[i]);
			// CGAL::Polygon_mesh_processing::stitch_boundary_cycles(meshes[i]);

			// if(i == 0)
			// 	set_mesh_color(meshes[i], CGAL::Color(200, 0, 0));
			// else if(i == 1)
			// 	set_mesh_color(meshes[i], CGAL::Color(0, 200, 0));
			// else
			// 	set_mesh_color(meshes[i], random_color());

			textures[i] = data.texture_path;
		}
	}

	std::vector<Surface_mesh> current_close(input_files.size() - 1);
	std::vector<Surface_mesh> current_distant(input_files.size() - 1);
	std::vector<Surface_mesh> current_projected(input_files.size() - 1);

	std::vector<Surface_mesh> next_close(input_files.size() - 1);
	std::vector<Surface_mesh> next_distant(input_files.size() - 1);
	std::vector<Surface_mesh> next_projected(input_files.size() - 1);

	Surface_mesh reconstruction = meshes[0];

	////////// MESH PROCESSING

	std::cerr << "Filtering with threshold = " << threshold << '\n';

	for(size_t i = 1; i < input_files.size(); ++i)
	{
		Surface_mesh current_mesh = reconstruction;
		Surface_mesh next_mesh	  = meshes[i];

		////////// CURRENT MESH

		// std::cerr << "[CURRENT_MESH] total vertices: " << current_mesh.number_of_vertices() <<
		// '\n';

		// std::cerr << "[CURRENT_MESH] Marking...\n";
		// auto current_marking_map = mark_delimited_regions(current_mesh, next_mesh, threshold);

		// std::cerr << "[CURRENT_MESH] Dividing...\n";
		// auto current_division = divide(current_mesh, current_marking_map);

		// current_close[i - 1]   = current_division.first;
		// current_distant[i - 1] = current_division.second;

		////////// PROJECTION

		std::cerr << "[CURRENT_MESH] Projecting...\n";
		current_projected[i - 1] = projection(current_mesh, next_mesh);

		////////// NEXT MESH

		std::cerr << "[NEXT_MESH] total vertices: " << next_mesh.number_of_vertices() << '\n';

		std::cerr << "[NEXT_MESH] Marking...\n";
		mark_delimited_regions(next_mesh, current_mesh, threshold);

		std::cerr << "[CURRENT_MESH] Marking...\n";
		mark_delimited_regions(current_projected[i - 1], next_mesh);

		////////// STICK CURRENT MESH

		std::cerr << "[CURRENT_MESH] sticking vertices...\n";

		current_projected[i - 1] =
			stick_vertices(current_projected[i - 1], limit_vertices(current_projected[i - 1]),
						   next_mesh, limit_vertices(next_mesh));

		std::cerr << "[CURRENT_MESH] filling holes...\n";

		// [WARNING] attention à ne pas confondre current_marking_map et projected_marking_map

		current_projected[i - 1] = fill_holes(current_projected[i - 1], next_mesh);

		// mark_delimited_regions(current_projected[i - 1], next_mesh);

		////////// COLORIZE MESHES

		set_mesh_color(current_projected[i - 1], close_vertices(current_projected[i - 1]),
					   CGAL::Color(150, 0, 0));
		set_mesh_color(current_projected[i - 1], limit_vertices(current_projected[i - 1]),
					   CGAL::Color(0, 150, 0));
		set_mesh_color(current_projected[i - 1], distant_vertices(current_projected[i - 1]),
					   CGAL::Color(0, 0, 150));

		set_mesh_color(next_mesh, close_vertices(next_mesh), CGAL::Color(200, 0, 0));
		set_mesh_color(next_mesh, limit_vertices(next_mesh), CGAL::Color(0, 200, 0));
		set_mesh_color(next_mesh, distant_vertices(next_mesh), CGAL::Color(0, 0, 200));

		////////// DIVIDE MESHES

		std::cerr << "[NEXT_MESH] Dividing...\n";
		auto next_division = divide(next_mesh);

		next_close[i - 1]	= next_division.first;
		next_distant[i - 1] = next_division.second;

		std::cerr << "[CURRENT_MESH] Dividing...\n";
		auto current_division = divide(current_projected[i - 1]);

		current_close[i - 1]   = current_division.first;
		current_distant[i - 1] = current_division.second;

		//////////// RENCONSTRUCTED MESH

		reconstruction = current_close[i - 1];
		reconstruction += next_distant[i - 1];
	}

	////////// MESH VISUALISATION

	std::cerr << "[STATUS] Allocating meshes on gpu...\n";

	int qargc			 = 1;
	const char* qargv[2] = {"surface_mesh_viewer", "\0"};

	QApplication application(qargc, const_cast<char**>(qargv));

	MeshViewer viewer;

	viewer.setWindowTitle("Surface_meshViewer");

	viewer.show(); // Create Opengl Context

	/// 6 : reconstruction

	// TODO: enlever triangle qui se superpose en parcourant les limites

	for(size_t i = 0; i < meshes.size() - 1; ++i)
	{
		// viewer.add(to_mesh_data(meshes[i], textures[i]));
		// viewer.add(to_mesh_data(meshes[i + 1], textures[i]));
		viewer.add(to_mesh_data(next_close[i], textures[i]));
		viewer.add(to_mesh_data(next_distant[i], textures[i]));

		viewer.add(to_mesh_data(current_close[i], textures[i]));
		viewer.add(to_mesh_data(current_distant[i], textures[i]));

		viewer.add(to_mesh_data(current_projected[i], textures[i]));
	}

	return application.exec();
	// return EXIT_SUCCESS;
}