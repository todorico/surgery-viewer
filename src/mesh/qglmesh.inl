#ifndef QGLMESH_INL
#define QGLMESH_INL

#include "qglmesh.hpp"

QGLMesh::QGLMesh()
	: vao(new QOpenGLVertexArrayObject()), vertices(QOpenGLBuffer::VertexBuffer),
	  normals(QOpenGLBuffer::VertexBuffer), indices(QOpenGLBuffer::IndexBuffer)
{
}

QGLMesh::QGLMesh(const Mesh_data& data) : QGLMesh()
{
	this->allocate(data);
}

QGLMesh::QGLMesh(const Mesh_data& data, QOpenGLShaderProgram& shader_program) : QGLMesh()
{
	this->allocate(data);
	this->use(shader_program);
}

void QGLMesh::allocate(const Mesh_data& data)
{
	std::cerr << "[DEBUG] Allocating vertex array object...\n";
	vao->create();
	vao->bind();
	{
		std::cerr << "[DEBUG] Allocating vertices buffer...\n";

		vertices.create();
		vertices.bind();
		vertices.setUsagePattern(QOpenGLBuffer::StaticDraw);
		vertices.allocate(data.vertices.data(),
						  data.vertices.size() * sizeof(decltype(data.vertices)::value_type));

		std::cerr << "[DEBUG] Allocating normals buffer...\n";

		normals.create();
		normals.bind();
		normals.setUsagePattern(QOpenGLBuffer::StaticDraw);
		normals.allocate(data.normals.data(),
						 data.normals.size() * sizeof(decltype(data.normals)::value_type));

		std::cerr << "[DEBUG] Allocating indices buffer...\n";

		indices.create();
		indices.bind();
		indices.allocate(data.indices.data(),
						 data.indices.size() * sizeof(decltype(data.indices)::value_type));

		if(data.texture_path.has_value())
		{
			uvs = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

			std::cerr << "[DEBUG] Allocating uvs buffer...\n";

			uvs->create();
			uvs->bind();
			uvs->setUsagePattern(QOpenGLBuffer::StaticDraw);
			uvs->allocate(data.uvs->data(),
						  data.uvs->size() * sizeof(decltype(data.uvs)::value_type::value_type));

			std::cerr << "[DEBUG] Loading texture from " << data.texture_path.value() << "\n";

			texture.reset(new QOpenGLTexture(QImage(data.texture_path->c_str()).mirrored()));

			texture->generateMipMaps();
			texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
			texture->setMagnificationFilter(QOpenGLTexture::Linear);
			texture->setWrapMode(QOpenGLTexture::Repeat);
		}
	}
	vao->release();
}

bool QGLMesh::use(QOpenGLShaderProgram& shader_program)
{
	if(shader_program.isLinked())
	{
		vao->bind();
		{
			if(vertices.isCreated())
			{
				vertices.bind();
				shader_program.enableAttributeArray("aPos");
				shader_program.setAttributeBuffer("aPos", GL_FLOAT, 0, 3);
			}

			if(normals.isCreated())
			{
				normals.bind();
				shader_program.enableAttributeArray("aNormal");
				shader_program.setAttributeBuffer("aNormal", GL_FLOAT, 0, 3);
			}

			if(uvs.has_value() && uvs->isCreated())
			{
				uvs->bind();
				shader_program.enableAttributeArray("aTexCoord");
				shader_program.setAttributeBuffer("aTexCoord", GL_FLOAT, 0, 2);
			}
		}
		vao->release();
		return true;
	}
	else
	{
		return false;
	}
}

void QGLMesh::draw(QOpenGLShaderProgram& shader_program)
{
	if(texture)
	{
		shader_program.setUniformValue("Texture", texture->textureId());
		texture->bind(texture->textureId());
	}

	vao->bind();
	{
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	vao->release();
}

#endif // QGLMESH_INL