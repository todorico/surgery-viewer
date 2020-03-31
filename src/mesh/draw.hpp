#ifndef MESH_DRAW_HPP
#define MESH_DRAW_HPP

// CGAL
#include <CGAL/Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>

struct CustomColorFunctorSM
{
	template <typename SM>
	static CGAL::Color run(SM& mesh, typename SM::Face_index fh)
	{
		auto [face_color_map, face_color_exist] =
			mesh.template property_map<typename SM::Face_index, CGAL::Color>("f:color");

		if(face_color_exist)
		{
			return face_color_map[fh];
		}

		auto [vertex_color_map, vertex_color_exist] =
			mesh.template property_map<typename SM::Vertex_index, CGAL::Color>("v:color");

		if(vertex_color_exist)
		{
			auto vi = mesh.source(mesh.halfedge(fh)); // Select first vertex of face

			return vertex_color_map[vi];
		}

		return CGAL::Color(100, 125, 200);
	}
};

template <class SM, class ColorFunctor>
class CustomSurfaceMeshViewerQt : public CGAL::SimpleSurfaceMeshViewerQt<SM, ColorFunctor>
{
	using CGAL::SimpleSurfaceMeshViewerQt<SM, ColorFunctor>::SimpleSurfaceMeshViewerQt;
};

template <class K>
void mesh_draw(const CGAL::Surface_mesh<K>& amesh, const char* title = "Surface_mesh Basic Viewer",
			   bool nofill = false)
{
#if defined(CGAL_TEST_SUITE)
	bool cgal_test_suite = true;
#else
	bool cgal_test_suite = qEnvironmentVariableIsSet("CGAL_TEST_SUITE");
#endif

	if(!cgal_test_suite)
	{
		int argc			= 1;
		const char* argv[2] = {"surface_mesh_viewer", "\0"};
		QApplication app(argc, const_cast<char**>(argv));
		CustomColorFunctorSM fcolor;
		CustomSurfaceMeshViewerQt<CGAL::Surface_mesh<K>, CustomColorFunctorSM> mainwindow(
			app.activeWindow(), amesh, title, nofill, fcolor);
		mainwindow.show();
		app.exec();
	}
}

// #include "draw.inl"

#endif // MESH_DRAW_HPP
