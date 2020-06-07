#ifndef MESH_VIEW_HPP
#define MESH_VIEW_HPP

// PROJECT

#include "viewer.hpp"

// CGAL

#include <CGAL/Surface_mesh.h>

// template <class SurfaceMesh>
// void view(SurfaceMesh meshes...)
// {
// 	QApplication application(argc, argv);

// 	MeshViewer viewer;

// 	viewer.setWindowTitle("surgery-viewer");

// 	viewer.show(); // Create Opengl Context

// 	std::cerr << "[DEBUG] Loading meshes...\n";

// 	viewer
// 	// for(auto file : input_files)
// 	// {
// 	// 	viewer.add(load_mesh_data(file));
// 	// }

// 	std::cerr << "[DEBUG] Mesh(es) loaded successfuly !\n";

// 	application.exec();
// }

template <typename... MeshData>
void view(MeshData&&... mesh_datas)
{
	int argc			= 1;
	const char* argv[2] = {"surface_mesh_viewer", "\0"};

	QApplication application(argc, const_cast<char**>(argv));

	MeshViewer viewer;

	viewer.setWindowTitle("MeshViewer");

	viewer.show(); // Create Opengl Context

	std::cerr << "[DEBUG] Loading meshes...\n";

	(viewer.add(std::forward<MeshData>(mesh_datas)), ...);

	std::cerr << "[DEBUG] Mesh(es) loaded successfuly !\n";

	application.exec();
}

#include "view.inl"

#endif // MESH_VIEW_HPP