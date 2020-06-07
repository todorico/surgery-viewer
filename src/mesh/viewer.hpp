#ifndef MESH_VIEWER_HPP
#define MESH_VIEWER_HPP

#include "qglmesh.hpp"

#include <CGAL/Qt/qglviewer.h>
// #include <QKeyEvent>

#include <array>
#include <memory>

class MeshViewer : public CGAL::QGLViewer
{
  public:
	std::vector<QGLMesh> meshes;

	virtual void add(const Mesh_data& data);

  protected:
	virtual void draw();
	virtual void init();
	virtual QString helpString() const;
	virtual void keyPressEvent(QKeyEvent* e);
	void load_texture(const std::string& filename);

	std::unique_ptr<QOpenGLShaderProgram> mesh_shader_program;

	std::array<bool, 10> m_draw_mesh;

	CGAL::qglviewer::Vec orig, dir, selectedPoint;
};

#include "viewer.inl"

#endif // MESH_VIEWER_HPP