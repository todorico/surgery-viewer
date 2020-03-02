#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/draw_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>

// default triangulation for Surface_mesher
typedef CGAL::Surface_mesh_default_triangulation_3 Tr;

// c2t3
typedef CGAL::Complex_2_in_triangulation_3<Tr> C2t3;

typedef Tr::Geom_traits GT;
typedef GT::Sphere_3 Sphere_3;
typedef GT::Point_3 Point_3;
typedef GT::FT FT;

typedef FT (*Function)(Point_3);

typedef CGAL::Implicit_surface_3<GT, Function> Surface_3;

// Sphere equation : x² + y² + z² - r² = 0
FT sphere_function(Point_3 p)
{
	const FT x2 = p.x() * p.x(), y2 = p.y() * p.y(), z2 = p.z() * p.z();
	return x2 + y2 + z2 - 1;
}

// Plane equation : ax + by + cz + d = 0
FT plane_function(Point_3 p)
{
	return p.z();
}

// Plane equation : x²/a² + y²/b² - z = 0
FT parabola_function(Point_3 p)
{
	return p.x() * p.x() + p.y() * p.y() - p.z();
}

int main(int argc, const char* argv[])
{
	if(argc < 2)
	{
		std::cerr << "Usage : " << argv[0]
				  << " <surface_function_type> <squared_radius> <angular_bound> <radius_bound> "
					 "<distance_bound> <initial_points_number>\n";
		return EXIT_FAILURE;
	}

	// Paramètres par default

	std::string surface_function_type = "sphere";
	auto surface_function			  = sphere_function;
	double squared_radius			  = 100.0;
	double angular_bound			  = 30.0;
	double radius_bound				  = 1.0;
	double distance_bound			  = 1.0;
	int initial_points_number		  = 20;
	// std::string meshing_tag			  = "non_manifold";

	if(argc > 1)
		surface_function_type = argv[1];
	if(argc > 2)
		squared_radius = atof(argv[2]);
	if(argc > 3)
		angular_bound = atof(argv[3]);
	if(argc > 4)
		radius_bound = atof(argv[4]);
	if(argc > 5)
		distance_bound = atof(argv[5]);
	if(argc > 6)
		initial_points_number = atoi(argv[6]);

	Tr tr; // 3D-Delaunay triangulation

	C2t3 c2t3(tr); // 2D-complex in 3D-Delaunay triangulation

	if(surface_function_type == "sphere")
	{
		surface_function = sphere_function;
	}
	else if(surface_function_type == "parabola")
	{
		surface_function = parabola_function;
	}
	else if(surface_function_type == "plane")
	{
		surface_function = plane_function;
	}
	else
	{
		std::cerr << "this surface function is not supported\n";
		return EXIT_FAILURE;
	}

	// defining the surface
	Surface_3 surface(surface_function,						   // pointer to function
					  Sphere_3(CGAL::ORIGIN, squared_radius)); // bounding sphere

	// Note that "2." above is the* squared* radius of the bounding sphere !defining meshing
	// criteria
	CGAL::Surface_mesh_default_criteria_3<Tr> criteria(angular_bound,   // angular bound
													   radius_bound,	// radius bound
													   distance_bound); // distance bound

	CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Non_manifold_tag(),
							initial_points_number);

	std::cout << "Final number of points: " << tr.number_of_vertices() << "\n";

	CGAL::draw(tr);

	return EXIT_SUCCESS;
}
