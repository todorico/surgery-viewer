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

	QOpenGLBuffer vertices;
	QOpenGLBuffer normals;
	QOpenGLBuffer indices;

	// FIX QOpenGLTexture::destroy error : called without à current context
	// must be caused because unique_ptr automaticaly destroy texture after context is destroyed.
	std::unique_ptr<QOpenGLTexture> texture;
	std::optional<QOpenGLBuffer> uvs;

	QGLMesh();
	QGLMesh(const Mesh_data& data);
	QGLMesh(const Mesh_data& data, QOpenGLShaderProgram& shader_program);

	// allocate data on gpu
	void allocate(const Mesh_data& data);

	// use shader program for rendering
	bool use(QOpenGLShaderProgram& shader_program);

	// program must be bound before draw
	void draw(QOpenGLShaderProgram& shader_program);
};

#include "qglmesh.inl"

#endif // QGLMESH