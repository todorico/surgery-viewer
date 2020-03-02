
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

// Plane equation : ax + by + cz + d = 0
FT plane_function(Point_3 p)
{
	return p.z();
}

// Plane equation : x²/a² + y²/b² - z = 0
FT parabola_function(Point_3 p)
{
	// const Point_3 final = p - Sphere_3(0, 0, )
	return p.x() * p.x() + p.y() * p.y() - p.z() - 2;
}

int main(int argc, const char* argv[])
{
	if(argc < 1)
	{
		std::cerr << "Usage : " << argv[0]
				  << " <center_x> <center_y> "
					 "<center_z>\n";
		return EXIT_FAILURE;
	}

	// Paramètres par default
	double angular_bound  = 33.3;
	double radius_bound   = 0.1;
	double distance_bound = 0.1;

	double x = 0, y = 0, z = 0;

	if(argc > 1)
		x = atof(argv[1]);
	if(argc > 2)
		y = atof(argv[2]);
	if(argc > 3)
		z = atof(argv[3]);

	Point_3 center(x, y, z);

	Tr surface_tr;
	CGAL::Surface_mesh_default_criteria_3<Tr> criteria(angular_bound,   // angular bound
													   radius_bound,	// radius bound
													   distance_bound); // distance bound
	C2t3 surface_c2t3(surface_tr); // 2D-complex in 3D-Delaunay triangulation

	std::cout << "Creation de la parabole...";
	Surface_3 parabola_surface(parabola_function, Sphere_3(center, 4));
	CGAL::make_surface_mesh(surface_c2t3, parabola_surface, criteria, CGAL::Non_manifold_tag());
	std::cout << "OK\n";

	std::cout << "Creation du plan...";
	Surface_3 plane_surface(plane_function, Sphere_3(center, 9));
	CGAL::make_surface_mesh(surface_c2t3, plane_surface, criteria, CGAL::Non_manifold_tag());
	std::cout << "OK\n";

	// Tr parabola_tr;
	// C2t3 parabola_c2t3(surface_tr);

	std::cout << "Nombre de points : " << surface_tr.number_of_vertices() << "\n";

	CGAL::draw(surface_tr);

	// Multiple draw
	// Mesh distance + cut

	return EXIT_SUCCESS;
}
