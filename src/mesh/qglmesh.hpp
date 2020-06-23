#ifndef QGLMESH_HPP
#define QGLMESH_HPP

#include "data.hpp"

// QT5

#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

// STD

#include <memory>

class QGLMesh
{
  public:
	std::unique_ptr<QOpenGLVertexArrayObject> vao;
	std::unique_ptr<QOpenGLTexture> texture;

	QOpenGLBuffer positions;
	QOpenGLBuffer normals;
	QOpenGLBuffer colors;
	QOpenGLBuffer texcoords;
	QOpenGLBuffer triangulated_faces;

	// FIX QOpenGLTexture::destroy error : called without à current context
	// must be caused because unique_ptr automaticaly destroy texture after context is destroyed.

	QGLMesh();
	QGLMesh(const Mesh_data& data);
	QGLMesh(const Mesh_data& data, QOpenGLShaderProgram& shader_program);

	// allocate data on gpu
	void allocate(const Mesh_data& data);

	// use shader program for rendering
	bool use(QOpenGLShaderProgram& shader_program);

	// program must be bound before draw
	void draw(QOpenGLShaderProgram& shader_program, GLenum mode = GL_TRIANGLES);

  protected:
	size_t m_number_of_vertices;
	size_t m_number_of_faces;
};

// #include "qglmesh.inl"

#endif // QGLMESH