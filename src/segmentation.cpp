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

// 	std::cerr << "Reading mesh...";

// 	std::ifstream input_file(argv[1]);

// 	if(!input_file)
// 	{
// 		std::cerr << "Error : cannot open file\n";
// 		return EXIT_FAILURE;
// 	}

// 	Mesh sm1;

// 	input_file >> sm1;

// 	if(sm1.is_empty())
// 	{
// 		std::cerr << "Error : surface mesh is empty\n";
// 		return EXIT_FAILURE;
// 	}

// 	std::cerr << "OK\n";

// 	CGAL::draw(sm1);

// 	return EXIT_SUCCESS;
// }

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/Face_filtered_graph.h>
#include <CGAL/boost/graph/copy_face_graph.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>
#include <CGAL/mesh_segmentation.h>
#include <fstream>
#include <iostream>
#include <sstream>
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> SM;
typedef boost::graph_traits<SM>::face_descriptor face_descriptor;
int main(int argc, char** argv)
{
	SM mesh;
	if(argc == 2)
	{
		std::ifstream input(argv[1]);
		input >> mesh;
	}
	else
	{
		std::ifstream cactus("data/cactus.off");
		cactus >> mesh;
	}
	typedef SM::Property_map<face_descriptor, double> Facet_double_map;
	Facet_double_map sdf_property_map;
	sdf_property_map = mesh.add_property_map<face_descriptor, double>("f:sdf").first;
	CGAL::sdf_values(mesh, sdf_property_map);
	// create a property-map for segment-ids
	typedef SM::Property_map<face_descriptor, std::size_t> Facet_int_map;
	Facet_int_map segment_property_map =
		mesh.add_property_map<face_descriptor, std::size_t>("f:sid").first;
	;
	// segment the mesh using default parameters for number of levels, and smoothing lambda
	// Any other scalar values can be used instead of using SDF values computed using the CGAL
	// function
	std::size_t number_of_segments =
		CGAL::segmentation_from_sdf_values(mesh, sdf_property_map, segment_property_map);
	typedef CGAL::Face_filtered_graph<SM> Filtered_graph;
	// print area of each segment and then put it in a Mesh and print it in an OFF file
	Filtered_graph segment_mesh(mesh, 0, segment_property_map);
	for(std::size_t id = 0; id < number_of_segments; ++id)
	{
		if(id > 0)
			segment_mesh.set_selected_faces(id, segment_property_map);
		std::cout << "Segment " << id
				  << "'s area is : " << CGAL::Polygon_mesh_processing::area(segment_mesh)
				  << std::endl;
		SM out;
		CGAL::copy_face_graph(segment_mesh, out);
		std::ostringstream oss;
		oss << "Segment_" << id << ".off";
		std::ofstream os(oss.str().data());
		os << out;
	}
	CGAL::draw(segment_mesh.graph());
}