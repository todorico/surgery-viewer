// #include <CGAL/Simple_cartesian.h>
// #include <CGAL/Surface_mesh.h>
// #include <CGAL/draw_surface_mesh.h>
// #include <fstream>

// typedef CGAL::Simple_cartesian<double> Kernel;
// typedef Kernel::Point_3 Point;
// typedef CGAL::Surface_mesh<Point> Mesh;

// int main(int argc, char* argv[])
// {
// 	if(argc < 2)
// 	{
// 		std::cerr << "Usage : " << argv[0] << " <MESH_FILE>\n";
// 		return EXIT_FAILURE;
// 	}

// 	std::cout << "Reading mesh...";

// 	std::ifstream input_file(argv[1]);

// 	if(!input_file)
// 	{
// 		std::cerr << "Error : cannot open file\n";
// 		return EXIT_FAILURE;
// 	}

// 	Mesh sm1;

// 	if(!read_off(input_file, sm1))
// 	{
// 		std::cerr << "Error : failed to read file\n";
// 		return EXIT_FAILURE;
// 	}

// 	if(sm1.is_empty())
// 	{
// 		std::cerr << "Error : surface mesh is empty\n";
// 		return EXIT_FAILURE;
// 	}

// 	std::cout << "OK\n";

// 	CGAL::draw(sm1);

// 	return EXIT_SUCCESS;
// }

// #include "pre-compiled.hpp"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>
#include <fstream>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> Mesh;

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		std::cerr << "Usage : " << argv[0] << " <MESH_FILE>\n";
		return EXIT_FAILURE;
	}

	std::cerr << "Reading mesh...";

	std::ifstream input_file(argv[1]);

	if(!input_file)
	{
		std::cerr << "Error : cannot open file\n";
		return EXIT_FAILURE;
	}

	Mesh sm1;

	// read_ply(input_file, sm1);
	input_file >> sm1;

	if(sm1.is_empty())
	{
		std::cerr << "Error : surface mesh is empty\n";
		return EXIT_FAILURE;
	}

	std::cerr << "OK\n";
	std::cerr << "Properties:\n";

	for(auto p : sm1.properties<Mesh::Vertex_index>())
		std::cerr << p << '\n';

	for(auto p : sm1.properties<Mesh::Edge_index>())
		std::cerr << p << '\n';

	for(auto p : sm1.properties<Mesh::Halfedge_index>())
		std::cerr << p << '\n';

	for(auto p : sm1.properties<Mesh::Face_index>())
		std::cerr << p << '\n';

	std::cerr << "Vertices colors:\n";

	// for(auto v : sm1.vertices())
	// {
	// 	Mesh::Property_map<Mesh::Vertex_index, CGAL::Color> color;
	// 	std::cerr << color[v] << '\n';
	// }

	// Mesh::Face_range face_range = sm1.faces;

	// for(auto face : sm1.faces())
	// {
	// 	face.
	// }

	CGAL::draw(sm1);

	// write_ply(std::cout, sm1);
	// std::cout << sm1;

	return EXIT_SUCCESS;
}

// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Point_set_3.h>
// #include <CGAL/Point_set_3/IO.h>
// #include <CGAL/draw_point_set_3.h>
// #include <fstream>

// typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
// typedef Kernel::FT FT;
// typedef Kernel::Point_3 Point;
// typedef std::array<unsigned char, 4> Color;

// // // Point with normal, color and intensity
// typedef std::tuple<Point, Color> PNCI;
// typedef CGAL::Nth_of_tuple_property_map<0, PNCI> Point_map;
// // typedef CGAL::Nth_of_tuple_property_map<1, PNCI> Normal_map;
// typedef CGAL::Nth_of_tuple_property_map<1, PNCI> Color_map;
// // typedef CGAL::Nth_of_tuple_property_map<3, PNCI> Intensity_map;

// typedef CGAL::Point_set_3<Point> Point_set;

// int main(int argc, char** argv)
// {
// 	std::ifstream f(argc > 1 ? argv[1] : "data/oni.xyz");
// 	Point_set point_set;

// 	f >> point_set;
// 	CGAL::draw(point_set);
// 	std::cout << point_set;

// 	// // f >> point_set;
// 	// // Reading input in XYZ format
// 	// if(!f || !CGAL::read_off_points(f, point_set.index_back_inserter(),
// 	// 								CGAL::parameters::point_map(point_set.point_push_map()),
// 	// 								std::make_tuple(Color_map(), CGAL::Construct_array())))
// 	// {
// 	// 	std::cerr << "Can't read input file " << (argc > 1 ? argv[1] : "data/oni.xyz") <<
// 	// std::endl; 	return EXIT_FAILURE;
// 	// }

// 	// for(std::size_t i = 0; i < point_set.size(); ++i)
// 	// {
// 	// 	const Point& p = get<0>(point_set[i]);
// 	// 	// const Vector& n = get<1>(points[i]);
// 	// 	const Color& c = get<1>(point_set[i]);
// 	// 	// int I = get<2>(point_set[i]);

// 	// 	// std::cerr << "Point(" << p << ") with normal (" << n << ") with color (" << c
// 	// 	// 		  << ") and intensity (" << I << ")\n";
// 	// 	std::cerr << "Point(" << p << ") with color (" << c << ")\n";
// 	// }

// 	return EXIT_SUCCESS;
// }

// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Polygon_mesh_processing/measure.h>
// #include <CGAL/Surface_mesh.h>
// #include <CGAL/boost/graph/Face_filtered_graph.h>
// #include <CGAL/boost/graph/copy_face_graph.h>
// #include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
// #include <CGAL/draw_surface_mesh.h>
// #include <CGAL/mesh_segmentation.h>
// #include <fstream>
// #include <iostream>
// #include <sstream>
// typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
// typedef CGAL::Surface_mesh<Kernel::Point_3> SM;
// typedef boost::graph_traits<SM>::face_descriptor face_descriptor;
// int main(int argc, char** argv)
// {
// 	SM mesh;
// 	if(argc == 2)
// 	{
// 		std::ifstream input(argv[1]);
// 		input >> mesh;
// 	}
// 	else
// 	{
// 		std::ifstream cactus("data/cactus.off");
// 		cactus >> mesh;
// 	}
// 	typedef SM::Property_map<face_descriptor, double> Facet_double_map;
// 	Facet_double_map sdf_property_map;
// 	sdf_property_map = mesh.add_property_map<face_descriptor, double>("f:sdf").first;
// 	CGAL::sdf_values(mesh, sdf_property_map);
// 	// create a property-map for segment-ids
// 	typedef SM::Property_map<face_descriptor, std::size_t> Facet_int_map;
// 	Facet_int_map segment_property_map =
// 		mesh.add_property_map<face_descriptor, std::size_t>("f:sid").first;
// 	;
// 	// segment the mesh using default parameters for number of levels, and smoothing lambda
// 	// Any other scalar values can be used instead of using SDF values computed using the CGAL
// 	// function
// 	std::size_t number_of_segments =
// 		CGAL::segmentation_from_sdf_values(mesh, sdf_property_map, segment_property_map);
// 	typedef CGAL::Face_filtered_graph<SM> Filtered_graph;
// 	// print area of each segment and then put it in a Mesh and print it in an OFF file
// 	Filtered_graph segment_mesh(mesh, 0, segment_property_map);
// 	for(std::size_t id = 0; id < number_of_segments; ++id)
// 	{
// 		if(id > 0)
// 			segment_mesh.set_selected_faces(id, segment_property_map);
// 		std::cout << "Segment " << id
// 				  << "'s area is : " << CGAL::Polygon_mesh_processing::area(segment_mesh)
// 				  << std::endl;
// 		SM out;
// 		CGAL::copy_face_graph(segment_mesh, out);
// 		std::ostringstream oss;
// 		oss << "Segment_" << id << ".off";
// 		std::ofstream os(oss.str().data());
// 		os << out;
// 	}
// 	CGAL::draw(segment_mesh.graph());
// }

// // STL includes.
// #include <cstdlib>
// #include <fstream>
// #include <iostream>
// #include <iterator>
// #include <string>
// #include <vector>
// // CGAL includes.
// #include <CGAL/Exact_predicates_exact_constructions_kernel.h>
// #include <CGAL/HalfedgeDS_vector.h>
// #include <CGAL/IO/Color.h>
// #include <CGAL/Iterator_range.h>
// #include <CGAL/Polyhedron_3.h>
// #include <CGAL/Shape_detection/Region_growing/Region_growing.h>
// #include <CGAL/Shape_detection/Region_growing/Region_growing_on_polygon_mesh.h>
// #include <CGAL/Surface_mesh.h>
// #include <CGAL/draw_surface_mesh.h>
// #include <CGAL/memory.h>
// // Type declarations.
// using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
// using FT = typename Kernel::FT;
// using Point_3 = typename Kernel::Point_3;
// using Color = CGAL::Color;
// // Choose the type of a container for a polygon mesh.
// #define USE_SURFACE_MESH
// #if defined(USE_SURFACE_MESH)
// using Polygon_mesh = CGAL::Surface_mesh<Point_3>;
// using Face_range = typename Polygon_mesh::Face_range;
// using Neighbor_query =
// CGAL::Shape_detection::Polygon_mesh::One_ring_neighbor_query<Polygon_mesh>; using Region_type =
// 	CGAL::Shape_detection::Polygon_mesh::Least_squares_plane_fit_region<Kernel, Polygon_mesh>;
// using Sorting =
// 	CGAL::Shape_detection::Polygon_mesh::Least_squares_plane_fit_sorting<Kernel, Polygon_mesh,
// 																		 Neighbor_query>;
// #else
// using Polygon_mesh = CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_3,
// CGAL::HalfedgeDS_vector>; using Face_range = 	typename CGAL::Iterator_range<typename
// boost::graph_traits<Polygon_mesh>::face_iterator>;

// using Neighbor_query =
// 	CGAL::Shape_detection::Polygon_mesh::One_ring_neighbor_query<Polygon_mesh, Face_range>;
// using Region_type =
// 	CGAL::Shape_detection::Polygon_mesh::Least_squares_plane_fit_region<Kernel, Polygon_mesh,
// 																		Face_range>;
// using Sorting = CGAL::Shape_detection::Polygon_mesh::Least_squares_plane_fit_sorting<
// 	Kernel, Polygon_mesh, Neighbor_query, Face_range>;
// #endif
// using Region = std::vector<std::size_t>;
// using Regions = std::vector<Region>;
// using Vertex_to_point_map = typename Region_type::Vertex_to_point_map;
// using Region_growing =
// 	CGAL::Shape_detection::Region_growing<Face_range, Neighbor_query, Region_type,
// 										  typename Sorting::Seed_map>;
// int main(int argc, char* argv[])
// {
// 	std::cout << std::endl
// 			  << "region_growing_on_polygon_mesh example started" << std::endl
// 			  << std::endl;
// 	// Load off data either from a local folder or a user-provided file.
// 	std::ifstream in(argc > 1 ? argv[1] : "data/polygon_mesh.off");
// 	CGAL::set_ascii_mode(in);
// 	Polygon_mesh polygon_mesh;
// 	in >> polygon_mesh;

// 	in.close();
// 	const Face_range face_range = faces(polygon_mesh);
// 	std::cout << "* polygon mesh with " << face_range.size() << " faces is loaded" << std::endl;
// 	// Default parameter values for the data file polygon_mesh.off.
// 	const FT max_distance_to_plane = FT(1);
// 	const FT max_accepted_angle = FT(45);
// 	const std::size_t min_region_size = 5;
// 	// Create instances of the classes Neighbor_query and Region_type.
// 	Neighbor_query neighbor_query(polygon_mesh);
// 	const Vertex_to_point_map vertex_to_point_map(get(CGAL::vertex_point, polygon_mesh));
// 	Region_type region_type(polygon_mesh, max_distance_to_plane, max_accepted_angle,
// 							min_region_size, vertex_to_point_map);
// 	// Sort face indices.
// 	Sorting sorting(polygon_mesh, neighbor_query, vertex_to_point_map);
// 	sorting.sort();
// 	// Create an instance of the region growing class.
// 	Region_growing region_growing(face_range, neighbor_query, region_type, sorting.seed_map());
// 	// Run the algorithm.
// 	Regions regions;
// 	region_growing.detect(std::back_inserter(regions));
// 	// Print the number of found regions.
// 	std::cout << "* " << regions.size() << " regions have been found" << std::endl;
// // Save the result in a file only if it is stored in CGAL::Surface_mesh.
// #if defined(USE_SURFACE_MESH)

// 	using Face_index = typename Polygon_mesh::Face_index;

// 	// Save the result to a file in the user-provided path if any.
// 	srand(static_cast<unsigned int>(time(NULL)));
// 	if(argc > 2)
// 	{
// 		bool created;
// 		typename Polygon_mesh::template Property_map<Face_index, Color> face_color;
// 		boost::tie(face_color, created) =
// 			polygon_mesh.template add_property_map<Face_index, Color>("f:color", Color(0, 0, 0));
// 		if(!created)
// 		{
// 			std::cout << std::endl
// 					  << "region_growing_on_polygon_mesh example finished" << std::endl
// 					  << std::endl;

// 			return EXIT_FAILURE;
// 		}
// 		const std::string path = argv[2];
// 		const std::string fullpath = path + "regions_polygon_mesh.off";
// 		std::ofstream out(fullpath);
// 		// Iterate through all regions.
// 		for(const auto& region : regions)
// 		{
// 			// Generate a random color.
// 			const Color color(static_cast<unsigned char>(rand() % 256),
// 							  static_cast<unsigned char>(rand() % 256),
// 							  static_cast<unsigned char>(rand() % 256));
// 			// Iterate through all region items.
// 			using size_type = typename Polygon_mesh::size_type;
// 			for(const auto index : region)
// 				face_color[Face_index(static_cast<size_type>(index))] = color;
// 		}
// 		out << polygon_mesh;
// 		out.close();
// 		std::cout << "* polygon mesh is saved in " << fullpath << std::endl;
// 	}
// #endif
// 	std::cout << std::endl
// 			  << "region_growing_on_polygon_mesh example finished" << std::endl
// 			  << std::endl;

// 	CGAL::draw(polygon_mesh);

// 	return EXIT_SUCCESS;
// }