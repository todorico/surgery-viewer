#ifndef MESH_IO_HPP
#define MESH_IO_HPP

// CGAL
#include <CGAL/Surface_mesh.h>

// STD
#include <fstream>
#include <string>

// Reading
template <class P>
bool write_mesh_to(std::string filename,
				   const CGAL::Surface_mesh<P>& sm); // Guess format based on filename extension
template <class P>
bool write_mesh_to(std::string filename, const CGAL::Surface_mesh<P>& sm, std::string format);
template <class P>
bool write_mesh_to(std::ostream& os, const CGAL::Surface_mesh<P>& sm, std::string format);

// Writing
template <class P>
bool read_mesh_from(std::string filename,
					const CGAL::Surface_mesh<P>& sm); // Guess format based on filename extension
template <class P>
bool read_mesh_from(std::istream& is, CGAL::Surface_mesh<P>& sm, std::string format);
template <class P>
bool read_mesh_from(std::string filename, CGAL::Surface_mesh<P>& sm, std::string format);

#include "io.inl"

#endif