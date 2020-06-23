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

template <class VertexRange>
bool is_distant_face(const VertexRange& face_vertices, const SM_marking_map& marking_map)
{
	size_t count_close = 0;

	for(auto v : face_vertices)
	{
		if(marking_map[v] == Vertex_mark::Close)
		{
			++count_close;
		}
	}

	return count_close == 0;
}

template <class VertexRange>
bool is_point_projected_on_triangle(const Surface_mesh& mesh, const VertexRange& triangle_vertices,
									const Kernel::Point_3& p)
{
	// if(triangle_vertices.size() != 3)
	// {
	// 	std::cerr << "[ERROR] triangle_vertices must be of size 3 but is of size "
	// 			  << triangle_vertices.size() << '\n';
	// 	exit(EXIT_FAILURE);
	// }

	// Get triangle points

	auto v_it = triangle_vertices.begin();

	auto a = mesh.point(*v_it);
	++v_it;
	auto b = mesh.point(*v_it);
	++v_it;
	auto c = mesh.point(*v_it);

	// Create triangle

	Kernel::Triangle_3 triangle(a, b, c);
	// auto p_projected = triangle.supporting_plane().projection(p);

	// Project point on triangle plane

	auto perpendicular_line = triangle.supporting_plane().perpendicular_line(p);
	// auto perpendicular_line = Kernel::Line_3(p, p_projected);
	// auto orthogonal_line

	return CGAL::do_intersect(perpendicular_line, triangle);

	// auto result = CGAL::intersection(perpendicular_line, triangle);

	// if(result.has_value())
	// {
	// 	if(auto segment = boost::get<Kernel::Segment_3>(&*result))
	// 	{
	// 		return segment->has_on(p);
	// 	}
	// 	else
	// 	{
	// 		// auto point = boost::get<Kernel::Point_3>(&*result);
	// 		return true;
	// 	}
	// }

	// return false;
	// auto dist = Kernel::Vector_3(p, p_projected).squared_length();
	// return dist < 0.00000001;
	// bool lol  = triangle.has_on(p_projected) || dist < 0.0000000001;

	// std::cerr << "on_triangle? " << lol << ", ";
	// std::cerr << "distance? " << dist << ", ";
	// return lol;
}

SM_marking_map mark_regions(Surface_mesh& M1, const Surface_mesh& M2, const SM_kd_tree& M2_tree,
							SM_marking_map& M2_marking_map)
{
	auto [marking_map, created] =
		M1.add_property_map<Surface_mesh::Vertex_index, Vertex_mark>("v:mark", Vertex_mark::None);

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

		// if(M2_marking_map[M2_v] == Vertex_mark::Distant)
		// 	marking_map[M1_v] = Vertex_mark::Distant;
		// else
		// 	marking_map[M1_v] = Vertex_mark::Close;

		// std::cerr << "before faces around target\n";

		auto faces = CGAL::halfedges_around_target(M2_v, M2);
		// auto faces = CGAL::halfedges_around_target(M1_v, M1);
		// auto faces = CGAL::halfedges_around_target(M1_v, M2);
		std::cerr << "nb faces : " << faces.size() << '\n';
		// std::cerr << "degree v : " << M2.degree(M2_v) << '\n';

		bool found_distant_triangle = false;

		// std::cerr << "before faces iteration\n";
		for(auto f : faces)
		{
			// std::cerr << "before vertices around face\n";
			// std::cerr << "is_border!\n";
			// {
			// size_t on_triangle_count = 0;
			//
			if(M2.is_border(f))
			{
				// 	// f = M2.opposite(f);
				std::cerr << "[WARNING] border\n";
				continue;
			}

			// M2_marking_map[M2.source(f)] = Vertex_mark::Limit;

			// if(!M2.is_border(f))
			// {
			auto M2_triangle_vertices = CGAL::vertices_around_face(f, M2);
			// auto M2_triangle_vertices = CGAL::vertices_around_face(f, M1);
			// std::cerr << "nb points : " << M2_triangle_vertices.size() << '\n';
			// // std::cerr << "face size : " << M2_triangle_vertices.size() << '\n';

			// for(auto v : M2_triangle_vertices)
			// {
			// 	M2_marking_map[v] = Vertex_mark::Limit;
			// 	// marking_map[v] = Vertex_mark::Limit;
			// }
			// std::cerr << "after vertices around face\n";
			if(is_point_projected_on_triangle(M2, M2_triangle_vertices, M1_point) &&
			   is_distant_face(M2_triangle_vertices, M2_marking_map))
			{
				// 	// on_triangle_count++;
				found_distant_triangle = true;
				break;
			}
		}
		// M2_marking_map[M2_v] = Vertex_mark::Distant;
		// marking_map[M1_v] = Vertex_mark::Distant;
		// std::cerr << "triangle process finished\n";

		if(found_distant_triangle)
			marking_map[M1_v] = Vertex_mark::Distant;
		else
			marking_map[M1_v] = Vertex_mark::Close;

		// if(M2_marking_map[M2_v] == Vertex_mark::Distant)
		// else
		// auto [M2_v2, M2_dist_squared2] = *(search.begin() + 1);

		// auto face1 = M2.halfedge(M2_v1, M2_v2);

		// std::cerr << "halfedge1: ";
		// std::cerr << "is_null? " << (face1 == Surface_mesh::null_halfedge()) << ", ";

		// if(face1 != Surface_mesh::null_halfedge())
		// 	std::cerr << "is_border? " << (M2.is_border(face1)) << ", ";

		// if(face1 != Surface_mesh::null_halfedge() && !M2.is_border(face1))
		// {
		// 	auto M2_triangle_vertices = CGAL::vertices_around_face(face1, M2);

		// 	if(is_point_projected_on_triangle(M2, M2_triangle_vertices, M1_point))
		// 	{
		// 		bool is_close = false;

		// 		for(auto M2_v : M2_triangle_vertices)
		// 		{
		// 			if(M2_marking_map[M2_v] == Vertex_mark::Close)
		// 			{
		// 				is_close = true;
		// 				break;
		// 			}
		// 		}

		// 		if(is_close)
		// 		{
		// 			std::cerr << "is_close\n";
		// 			marking_map[M1_v] = Vertex_mark::Close;
		// 			continue;
		// 		}
		// 	}
		// }

		// std::cerr << "\nhalfedge2: ";
		// auto face2 = M2.halfedge(M2_v2, M2_v1);
		// std::cerr << "is_null? " << (face2 == Surface_mesh::null_halfedge()) << ", ";

		// if(face1 != Surface_mesh::null_halfedge())
		// 	std::cerr << "is_border? " << (M2.is_border(face2)) << ", ";

		// if(face2 != Surface_mesh::null_halfedge() && !M2.is_border(face2))
		// {
		// 	auto M2_triangle_vertices = CGAL::vertices_around_face(face2, M2);

		// 	if(is_point_projected_on_triangle(M2, M2_triangle_vertices, M1_point))
		// 	{
		// 		bool is_close = false;

		// 		for(auto M2_v : M2_triangle_vertices)
		// 		{
		// 			if(M2_marking_map[M2_v] == Vertex_mark::Close)
		// 			{
		// 				is_close = true;
		// 				break;
		// 			}
		// 		}

		// 		if(is_close)
		// 		{
		// 			std::cerr << "is_close\n";
		// 			marking_map[M1_v] = Vertex_mark::Close;
		// 			continue;
		// 		}
		// 	}
		// }

		// marking_map[M1_v] = Vertex_mark::Distant;
		// 	std::cerr
		// << "is_distant\n";
	}

	return marking_map;
}

SM_marking_map mark_regions(Surface_mesh& M1, const Surface_mesh& M2,
							SM_marking_map& M2_marking_map)
{
	SM_kd_tree M2_tree(M2.vertices().begin(), M2.vertices().end(), SM_kd_tree_splitter(),
					   SM_kd_tree_traits_adapter(M2.points()));

	return mark_regions(M1, M2, M2_tree, M2_marking_map);
}

SM_marking_map mark_delimited_regions(Surface_mesh& M1, const Surface_mesh& M2,
									  SM_marking_map& M2_marking_map)
{
	auto marking_map = mark_regions(M1, M2, M2_marking_map);
	return mark_limits(M1, marking_map);
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
		auto next_marking_map = mark_delimited_regions(next_mesh, current_mesh, threshold);

		std::cerr << "[CURRENT_MESH] Marking...\n";
		auto current_marking_map =
			mark_delimited_regions(current_projected[i - 1], next_mesh, next_marking_map);

		////////// COLORIZE MESHES

		set_mesh_color(current_projected[i - 1],
					   close_vertices(current_projected[i - 1].vertices(), current_marking_map),
					   CGAL::Color(150, 0, 0));
		set_mesh_color(current_projected[i - 1],
					   limit_vertices(current_projected[i - 1].vertices(), current_marking_map),
					   CGAL::Color(0, 150, 0));
		set_mesh_color(current_projected[i - 1],
					   distant_vertices(current_projected[i - 1].vertices(), current_marking_map),
					   CGAL::Color(0, 0, 150));

		set_mesh_color(next_mesh, close_vertices(next_mesh.vertices(), next_marking_map),
					   CGAL::Color(200, 0, 0));
		set_mesh_color(next_mesh, limit_vertices(next_mesh.vertices(), next_marking_map),
					   CGAL::Color(0, 200, 0));
		set_mesh_color(next_mesh, distant_vertices(next_mesh.vertices(), next_marking_map),
					   CGAL::Color(0, 0, 200));

		////////// DIVIDE MESHES

		std::cerr << "[NEXT_MESH] Dividing...\n";
		auto next_division = divide(next_mesh, next_marking_map);

		next_close[i - 1]	= next_division.first;
		next_distant[i - 1] = next_division.second;

		std::cerr << "[CURRENT_MESH] Dividing...\n";
		auto current_division = divide(current_projected[i - 1], current_marking_map);

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

		// viewer.add(to_mesh_data(current_projected[i], textures[i]));
	}

	return application.exec();
	// return EXIT_SUCCESS;
}