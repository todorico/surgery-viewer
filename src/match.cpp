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
				// std::cerr << "[WARNING] skipping border halfedge\n";
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
			// std::cerr << "[WARNING] could not found matching triangle for vertex " << M1_v <<
			// '\n';
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
	mark_regions(M1, M2);
	return mark_limits(M1);
}

template <class VertexRange>
Surface_mesh stick_vertices(const Surface_mesh& M1, const VertexRange& M1_vertices,
							const Surface_mesh& M2, const SM_kd_tree& M2_tree)
{
	Surface_mesh result = M1;

	auto M1_marking_map = get_marking_map(result);
	auto M2_marking_map = get_marking_map(M2);

	for(auto M1_v : M1_vertices)
	{
		SM_kd_tree_search search(M2_tree, result.point(M1_v), 1, 0, true,
								 M2_tree.traits().point_property_map());

		auto [M2_v, M2_dist_squared] = *(search.begin());

		result.point(M1_v)	 = M2_tree.traits().point_property_map()[M2_v];
		M1_marking_map[M1_v] = M2_marking_map[M2_v];
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

template <class VertexRange>
auto closest_vertices(const Surface_mesh& M1, const VertexRange& M1_vertices,
					  const SM_kd_tree& M2_tree)
{
	std::vector<Surface_mesh::Vertex_index> result;
	result.reserve(M1_vertices.size());

	for(auto M1_v : M1_vertices)
	{
		SM_kd_tree_search search(M2_tree, M1.point(M1_v), 1, 0, true,
								 M2_tree.traits().point_property_map());

		result.push_back(search.begin()->first);
	}

	return result;
}

template <class VertexRange1, class VertexRange2>
auto closest_vertices(const Surface_mesh& M1, const VertexRange1& M1_vertices,
					  const Surface_mesh& M2, const VertexRange2& M2_vertices)
{
	SM_kd_tree M2_tree(M2_vertices.begin(), M2_vertices.end(), SM_kd_tree_splitter(),
					   SM_kd_tree_traits_adapter(M2.points()));

	return closest_vertices(M1, M1_vertices, M2_tree);
}

Surface_mesh fill_holes(const Surface_mesh& M1, const Surface_mesh& M2)
{
	// point limites de M2 qui ne sont pas collé a M1
	auto M2_not_sticked_vertices =
		not_sticked_vertices(M2, limit_vertices(M2), M1, distant_vertices(M1));

	auto M1_closest_vertices_from_M2_not_sticked =
		closest_vertices(M2, M2_not_sticked_vertices, M1, M1.vertices());

	auto result =
		stick_vertices(M1, M1_closest_vertices_from_M2_not_sticked, M2, M2_not_sticked_vertices);

	return result;
	///

	// auto M1_not_sticked_vertices =
	// 	not_sticked_vertices(result, limit_vertices(result), M2, distant_vertices(M2));

	// auto M2_closest_vertices_from_M1_not_sticked =
	// 	closest_vertices(result, M1_not_sticked_vertices, M2, M2.vertices());

	// return stick_vertices(result, M1_not_sticked_vertices, M2,
	// 					  M2_closest_vertices_from_M1_not_sticked);
}

static const char USAGE[] =
	R"(Create a new mesh by matching parts of multiple similars meshes.

    Usage: match [options] <threshold> <input-files>...

    Options:
      -c, --colorize                     Colorize geometrical objects by files.
      -e <dist>, --epsilon <dist>		 Reduce threshold to make transition.
      --neighbors=<num>                  Numbers of neighbors to query during projection
      --iterations=<num>                 Numbers of iterations during projection
      -h --help                          Show this screen
      --version                          Show version
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

	auto opt_colorize = args.at("--colorize").asBool();

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

	double epsilon	 = 0;
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

	////////// MESH IMPORTATION

	std::vector<Surface_mesh> meshes(input_files.size());
	std::vector<std::optional<std::string>> textures(input_files.size());

	std::cerr << "[STATUS] Importing meshes from files...\n";
	{
		for(size_t i = 0; i < input_files.size(); ++i)
		{
			Mesh_data data = load_mesh_data(input_files[i]);

			textures[i] = data.texture_path;
			meshes[i]	= to_surface_mesh(data);

			// WARNING: force mesh to be geometricaly processable by removing duplicated halfedges
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
		}
	}

	Surface_mesh reconstruction = meshes[0];
	Surface_mesh current_projected;

	Surface_mesh M1_projected_close;
	Surface_mesh M1_projected_distant;

	// Surface_mesh M1_close_close;
	// Surface_mesh M1_close_distant;
	// Surface_mesh M1_close_distant_proj;

	Surface_mesh M2_close;
	Surface_mesh M2_distant;

	////////// MESH PROCESSING

	std::cerr << "Filtering with threshold = " << threshold << '\n';

	for(size_t i = 1; i < input_files.size(); ++i)
	{
		Surface_mesh current = reconstruction;
		Surface_mesh next	 = meshes[i];

		//////////////////////////////// OLD VERSION

		// mark_delimited_regions(current, next, threshold);
		// mark_delimited_regions(next, current, threshold);

		// auto current_div = divide(current);
		// auto next_div	 = divide(next);

		// mark_delimited_regions(current_div.first, next, threshold - epsilon);
		// mark_delimited_regions(next_div.first, current, threshold - epsilon);

		// auto current_first_div = divide(current_div.first);
		// auto next_first_div	   = divide(next_div.first);

		// M2_plus		   = next_div.second;
		// M1_minus	   = current_div.first;
		// M1_minus_minus = current_first_div.first;
		// M1_minus_trans = current_first_div.second;

		// M2_minus_trans = next_first_div.second;

		// M1_minus_trans_proj = projection(M1_minus, M1_minus_trans.vertices(), M2_minus_trans,
		// 								 M2_minus_trans.vertices());

		// set_mesh_color(M1_minus_trans_proj, M1_minus_trans.vertices(), CGAL::Color(200, 200, 0));

		//////////////////////////////// NEW VERSION

		////////// CURRENT MESH

		// std::cerr << "[CURRENT] total vertices: " << current.number_of_vertices() <<
		// '\n';

		// std::cerr << "[CURRENT] Marking...\n";
		// auto current_marking_map = mark_delimited_regions(current, next, threshold);

		// std::cerr << "[CURRENT] Dividing...\n";
		// auto current_division = divide(current, current_marking_map);

		// current_close[i - 1]   = current_division.first;
		// current_distant[i - 1] = current_division.second;

		////////// PROJECTION

		std::cerr << "[CURRENT] total vertices: " << current.number_of_vertices() << '\n';

		std::cerr << "[CURRENT] Projecting...\n";
		current_projected = projection(current, next);
		// projected				 = current_projected[i - 1];

		////////// NEXT MESH

		std::cerr << "[NEXT] total vertices: " << next.number_of_vertices() << '\n';

		std::cerr << "[NEXT] Marking...\n";
		mark_delimited_regions(next, current, threshold);

		std::cerr << "[CURRENT] Marking...\n";
		mark_delimited_regions(current_projected, next);

		if(opt_colorize)
		{
			set_mesh_color(next, limit_vertices(next), CGAL::Color(200, 200, 0));
			set_mesh_color(current_projected, limit_vertices(current_projected),
						   CGAL::Color(200, 200, 0));
		}

		////////// STICK CURRENT MESH

		std::cerr << "[CURRENT] filling holes...\n";

		// [WARNING] attention à ne pas confondre current_marking_map et projected_marking_map

		// current_projected[i - 1] = fill_holes(current_projected[i - 1], next);

		// mark_delimited_regions(current_projected[i - 1], next);

		////////// COLORIZE MESHES

		// set_mesh_color(current_projected[i - 1], close_vertices(current_projected[i - 1]),
		// 			   CGAL::Color(200, 0, 200));
		// set_mesh_color(current_projected[i - 1], limit_vertices(current_projected[i - 1]),
		//		   CGAL::Color(200, 200, 0));
		// set_mesh_color(current_projected[i - 1], distant_vertices(current_projected[i - 1]),
		//		   CGAL::Color(0, 200, 0));

		// set_mesh_color(next, close_vertices(next), CGAL::Color(200, 0, 200));
		// set_mesh_color(next, limit_vertices(next), CGAL::Color(200, 200, 0));
		// set_mesh_color(next, distant_vertices(next), CGAL::Color(0, 200, 0));

		////////// DIVIDE MESHES

		std::cerr << "[NEXT] Dividing...\n";
		std::tie(M2_close, M2_distant)					   = divide(next);
		std::tie(M1_projected_close, M1_projected_distant) = divide(current_projected);

		std::cerr << "[NEXT_DISTANT] total vertices: " << M2_distant.number_of_vertices() << '\n';
		std::cerr << "[NEXT_CLOSE] total vertices: " << M2_close.number_of_vertices() << '\n';

		// next_close[i - 1]	= next_division.first;
		// next_distant[i - 1] = next_division.second;

		// std::cerr << "[CURRENT] Dividing...\n";
		// auto current_division = divide(current_projected[i - 1]);

		// current_close[i - 1]   = current_division.first;
		// current_distant[i - 1] = current_division.second;

		// //////////// RENCONSTRUCTED MESH

		// reconstruction = current_close[i - 1];
		// reconstruction += next_distant[i - 1];
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

	// for(size_t i = 0; i < meshes.size() - 1; ++i)
	// {
	// 	viewer.add(to_mesh_data(meshes[i], textures[i]));
	// 	viewer.add(to_mesh_data(meshes[i + 1], textures[i]));

	// 	viewer.add(to_mesh_data(next_close[i], textures[i]));
	// 	viewer.add(to_mesh_data(next_distant[i], textures[i]));

	// 	viewer.add(to_mesh_data(current_close[i], textures[i]));
	// 	viewer.add(to_mesh_data(current_distant[i], textures[i]));

	// 	viewer.add(to_mesh_data(current_projected[i], textures[i]));
	// 	viewer.add(to_mesh_data(projected, textures[i]));
	// }

	viewer.add(to_mesh_data(meshes[0], textures[0]));
	viewer.add(to_mesh_data(meshes[1], textures[1]));

	viewer.add(to_mesh_data(M1_projected_close, textures[0]));
	viewer.add(to_mesh_data(M1_projected_distant, textures[0]));

	viewer.add(to_mesh_data(M2_close, textures[1]));
	viewer.add(to_mesh_data(M2_distant, textures[1]));

	return application.exec();
	// return EXIT_SUCCESS;
}