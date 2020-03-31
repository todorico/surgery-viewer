#ifndef MESH_IO_INL
#define MESH_IO_INL

// CGAL
#include <CGAL/Surface_mesh.h>

// STD
#include <fstream>
#include <string>

// 1. Add a head file. (#include <CGAL/IO/print_wavefront.h>)
// 2. Create an output stream with the file name you want to output.(std::ofstream
// ofs("MeshFile.obj");)
// 3. Call the output function.(CGAL::print_polyhedron_wavefront(ofs, mesh);), where mesh is a
// Polyhedron class.

// vout << "CGAL::generic_copy_OFF( " << iname << ", " << oname << ") ...." << endl;
// CGAL::File_writer_wavefront writer;
// CGAL::generic_copy_OFF(*p_in, *p_out, writer);
// vout << "    .... done." << endl;

// Reading
template <class P>
bool read_mesh_from(std::string filename, CGAL::Surface_mesh<P>& sm)
{
	const size_t pos = filename.rfind('.');

	if(pos == std::string::npos)
	{
		std::cerr << "Error: cannot deduce format from filename without extension\n";
		return false;
	}

	const std::string extension = filename.substr(pos + 1);

	return read_mesh_from(filename, sm, extension);
}

template <class P>
bool read_mesh_from(std::string filename, CGAL::Surface_mesh<P>& sm, std::string format)
{
	std::ifstream input_file(filename);

	if(!input_file.is_open())
	{
		std::cerr << "Error: cannot open input file\n";
		return false;
	}

	return read_mesh_from(input_file, sm, format);
}

template <class P>
bool read_mesh_from(std::istream& is, CGAL::Surface_mesh<P>& sm, std::string format)
{
	std::transform(format.begin(), format.end(), format.begin(), ::tolower);

	if(format == "off")
	{
		return CGAL::read_off(is, sm);
	}
	else if(format == "ply")
	{
		return CGAL::read_ply(is, sm);
	}
	else
	{
		std::cerr << "Error: unsupported input format\n";
		return false;
	}
}

// Writing
template <class P>
bool write_mesh_to(std::string filename, const CGAL::Surface_mesh<P>& sm)
{
	const size_t pos = filename.rfind('.');

	if(pos == std::string::npos)
	{
		std::cerr << "Error: cannot deduce format from filename without extension\n";
		return false;
	}

	const std::string extension = filename.substr(pos + 1);

	return write_mesh_to(filename, sm, extension);
}

template <class P>
bool write_mesh_to(std::string filename, const CGAL::Surface_mesh<P>& sm, std::string format)
{
	std::ofstream output_file(filename);

	if(!output_file.is_open())
	{
		std::cerr << "Error: cannot open output file\n";
		return false;
	}

	return write_mesh_to(output_file, sm, format);
}

template <class P>
bool write_mesh_to(std::ostream& os, const CGAL::Surface_mesh<P>& sm, std::string format)
{
	std::transform(format.begin(), format.end(), format.begin(), ::tolower);

	if(format == "off")
	{
		return CGAL::write_off(os, sm);
	}
	else if(format == "ply")
	{
		return CGAL::write_ply(os, sm);
	}
	else
	{
		std::cerr << "Error: unsupported output format\n";
		return false;
	}
}

#endif
