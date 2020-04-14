// Project
#include "docopt/docopt.h"
#include "mesh/io.hpp"

// CGAL
#include <CGAL/Aff_transformation_3.h>
#include <CGAL/Polygon_mesh_processing/transform.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
// STD
#include <iostream>
#include <random>

using Kernel = CGAL::Simple_cartesian<double>;
// using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
// using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;

using Point			 = Kernel::Point_3;
using Vector		 = Kernel::Vector_3;
using Transformation = Kernel::Aff_transformation_3;
using Mesh			 = CGAL::Surface_mesh<Point>;

static const char USAGE[] =
	R"(Mesh manipulation utility from geometry file (OFF, PLY, OBJ).

    Usage: manip <input-file> (<output-file> | --to=<format>) [options]

    Options:
      --color=<rgb>        Set mesh color
      --noisify=<float>    Add random noise to mesh
      --rotate-x=<float>   Rotate mesh around x axis by an angle in degree
      --rotate-y=<float>   Rotate mesh around y axis by an angle in degree
      --rotate-z=<float>   Rotate mesh around z axis by an angle in degree
      --to=<format>        Output <format> conversion to stdout.
      --translate=<vector> Translate mesh with vector
      -h --help            Show this screen.
      --version            Show version.
)";

Mesh noisification(const Mesh& mesh, double noise)
{
	Mesh res = mesh;

	std::random_device rd;
	std::default_random_engine re(rd());
	std::uniform_real_distribution ud(0.0, noise);

	for(auto vi : res.vertices())
	{
		Vector v(ud(re), ud(re), ud(re));
		res.point(vi) += v;
	}

	return res;
}

Mesh translation(const Mesh& mesh, const Vector& v)
{
	Mesh res = mesh;

	for(auto vi : res.vertices())
	{
		res.point(vi) += v;
	}

	return res;
}

Mesh rotation_z(const Mesh& mesh, double theta)
{
	Mesh res = mesh;

	Transformation rotation_z(cos(theta), -sin(theta), 0.0, sin(theta), cos(theta), 0.0, 0.0, 0.0,
							  1.0, 1.0);

	CGAL::Polygon_mesh_processing::transform(rotation_z, res);

	return res;
}

int main(int argc, char const* argv[])
{
	std::map<std::string, docopt::value> args =
		docopt::docopt(USAGE, {argv + 1, argv + argc}, true, "v1.0");

	Mesh mesh;
	std::string input_file = args.at("<input-file>").asString();

	std::cerr << "Reading mesh from " << input_file << "...\n";
	read_mesh_from(input_file, mesh);

	std::cerr << "nose options\n";
	auto noise_arg = args.at("--noisify");
	// std::cerr << "args end: " << &args.end() << "\n";

	if(noise_arg)
	{
		std::cerr << "diff null\n";
		try
		{
			double noise = stod(noise_arg.asString());
			std::cerr << "noise-value: " << noise << '\n';
			mesh = noisification(mesh, noise);
		}
		catch(std::invalid_argument& ia)
		{
			std::cerr << "Error: <noise-value> must be a real number\n";
			return EXIT_FAILURE;
		}
	}

	// std::cerr << "angle options\n";
	// auto angle_arg = args.at("--rotate");

	// if(angle_arg)
	// {
	// 	try
	// 	{
	// 		double angle = stod(angle_arg.asString());
	// 		mesh		 = rotation_z(mesh, angle);
	// 	}
	// 	catch(std::invalid_argument& ia)
	// 	{
	// 		std::cerr << "Error: <angle-value> must be a real number\n";
	// 		return EXIT_FAILURE;
	// 	}
	// }

	auto output_file_value = args.at("<output-file>");

	if(output_file_value)
	{
		std::string output_file = output_file_value.asString();

		std::cerr << "Writing mesh to " << output_file << "...\n";
		if(write_mesh_to(output_file, mesh))
			return EXIT_SUCCESS;
	}
	else
	{
		std::string format = args.at("--to").asString();

		std::cerr << "Writing mesh to stdout...\n";
		if(write_mesh_to(std::cout, mesh, format))
			return EXIT_SUCCESS;
	}
}