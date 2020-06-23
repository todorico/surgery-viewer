#ifndef MESH_VIEWER_HPP
#define MESH_VIEWER_HPP

#include "qglmesh.hpp"

#include <CGAL/Qt/qglviewer.h>

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
	virtual void initShaders();
	virtual QString helpString() const;
	virtual void keyPressEvent(QKeyEvent* e);
	void load_texture(const std::string& filename);
	bool GLLogErrors();

	QOpenGLShaderProgram* used_shader_program = nullptr;

	std::unique_ptr<QOpenGLShaderProgram> shader_program_color_only;
	std::unique_ptr<QOpenGLShaderProgram> shader_program_color_and_texture;
	std::unique_ptr<QOpenGLShaderProgram> shader_program_texture_only;
	// QOpenGLShaderProgram mesh_shader_program;

	std::array<bool, 12> m_draw_mesh;

	bool m_draw_triangles = true;
	bool m_draw_edges	  = false;
	bool m_draw_points	  = false;

	CGAL::qglviewer::Vec orig, dir, selectedPoint;
	// CGAL::qglviewer::Vec bb_min(std::numeric_limits<qreal>::max(),
	// std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()), bb_max;
};

// #include "viewer.inl"

#endif // MESH_VIEWER_HPP