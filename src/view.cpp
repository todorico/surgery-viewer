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

	input_file >> sm1;

	if(sm1.is_empty())
	{
		std::cerr << "Error : surface mesh is empty\n";
		return EXIT_FAILURE;
	}

	std::cerr << "OK\n";

	CGAL::draw(sm1);

	return EXIT_SUCCESS;
}