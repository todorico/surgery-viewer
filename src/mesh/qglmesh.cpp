// #ifndef QGLMESH_INL
// #define QGLMESH_INL

#include "qglmesh.hpp"

#include <iostream>

QGLMesh::QGLMesh()
	: vao(new QOpenGLVertexArrayObject()), texture(), positions(QOpenGLBuffer::VertexBuffer),
	  normals(QOpenGLBuffer::VertexBuffer), colors(QOpenGLBuffer::VertexBuffer),
	  texcoords(QOpenGLBuffer::VertexBuffer), triangulated_faces(QOpenGLBuffer::IndexBuffer)
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
		if(data.positions.has_value())
		{
			m_number_of_vertices = data.positions->size();

			std::cerr << "[DEBUG] Allocating buffer of " << m_number_of_vertices
					  << " positions...\n";

			positions.create();
			positions.bind();
			positions.setUsagePattern(QOpenGLBuffer::StaticDraw);
			positions.allocate(
				data.positions->data(),
				static_cast<int>(data.positions->size() *
								 sizeof(decltype(data.positions)::value_type::value_type)));
		}
		else
		{
			std::cerr << "[WARNING] No positions buffer allocated\n";
		}

		if(data.normals.has_value())
		{
			std::cerr << "[DEBUG] Allocating buffer of " << data.normals->size() << " normals...\n";

			normals.create();
			normals.bind();
			normals.setUsagePattern(QOpenGLBuffer::StaticDraw);
			normals.allocate(
				data.normals->data(),
				static_cast<int>(data.normals->size() *
								 sizeof(decltype(data.normals)::value_type::value_type)));
		}
		else
		{
			std::cerr << "[WARNING] No normals buffer allocated\n";
		}

		if(data.colors.has_value())
		{
			std::cerr << "[DEBUG] Allocating buffer of " << data.colors->size() << " colors...\n";

			colors.create();
			colors.bind();
			colors.setUsagePattern(QOpenGLBuffer::StaticDraw);
			colors.allocate(
				data.colors->data(),
				static_cast<int>(data.colors->size() *
								 sizeof(decltype(data.colors)::value_type::value_type)));
		}
		else
		{
			std::cerr << "[WARNING] No colors buffer allocated\n";
		}

		if(data.texcoords.has_value())
		{
			std::cerr << "[DEBUG] Allocating buffer of " << data.texcoords->size()
					  << " texcoords...\n";

			texcoords.create();
			texcoords.bind();
			texcoords.setUsagePattern(QOpenGLBuffer::StaticDraw);
			texcoords.allocate(
				data.texcoords->data(),
				static_cast<int>(data.texcoords->size() *
								 sizeof(decltype(data.texcoords)::value_type::value_type)));
		}
		else
		{
			std::cerr << "[WARNING] No texcoords buffer allocated\n";
		}

		if(data.texture_path.has_value())
		{
			std::cerr << "[DEBUG] Loading texture from " << data.texture_path.value() << "...\n";

			// QImage image_data = QImage(data.texture_path->c_str());

			// std::cerr << "[DEBUG] format wxh : " << image_data.width() << "x" << image_data.height() << '\n';

            // texture.reset(new QOpenGLTexture(image_data.mirrored()));
            texture.reset(new QOpenGLTexture(QImage(data.texture_path->c_str()).mirrored()));

			texture->generateMipMaps();
			texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
			texture->setMagnificationFilter(QOpenGLTexture::Linear);
			texture->setWrapMode(QOpenGLTexture::Repeat);
		}
		else
		{
			std::cerr << "[WARNING] No texture allocated\n";
		}

		if(data.triangulated_faces.has_value())
		{
			m_number_of_faces = data.triangulated_faces->size();

			std::cerr << "[DEBUG] Allocating buffer of " << m_number_of_faces << " faces...\n";

			triangulated_faces.create();
			triangulated_faces.bind();
			// triangulated_faces.setUsagePattern(QOpenGLBuffer::StaticDraw);
			triangulated_faces.allocate(
				data.triangulated_faces->data(),
				static_cast<int>(
					data.triangulated_faces->size() *
					sizeof(decltype(data.triangulated_faces)::value_type::value_type)));
		}
		else
		{
			std::cerr << "[WARNING] No triangulated_faces buffer allocated\n";
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
			if(positions.isCreated())
			{
				std::cerr << "[DEBUG] Attribute : position enabled\n";
				positions.bind();
				shader_program.enableAttributeArray("v_position");
				shader_program.setAttributeBuffer("v_position", GL_FLOAT, 0, 3);
			}

			if(normals.isCreated())
			{
				std::cerr << "[DEBUG] Attribute : normal enabled\n";
				normals.bind();
				shader_program.enableAttributeArray("v_normal");
				shader_program.setAttributeBuffer("v_normal", GL_FLOAT, 0, 3);
			}

			if(colors.isCreated())
			{
				std::cerr << "[DEBUG] Attribute : color enabled\n";
				colors.bind();
				shader_program.enableAttributeArray("v_color");
				shader_program.setAttributeBuffer("v_color", GL_FLOAT, 0, 4);
			}

			if(texcoords.isCreated())
			{
				std::cerr << "[DEBUG] Attribute : texcoords enabled\n";
				texcoords.bind();
				shader_program.enableAttributeArray("v_texcoord");
				shader_program.setAttributeBuffer("v_texcoord", GL_FLOAT, 0, 2);
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

void QGLMesh::draw(QOpenGLFunctions& gl, QOpenGLShaderProgram& shader_program, GLenum mode)
{
    if(texture)
    {
        shader_program.setUniformValue("f_texture", texture->textureId());
        texture->bind(texture->textureId());
    }

    vao->bind();
    {
        if(triangulated_faces.isCreated())
        {
            gl.glDrawElements(mode, static_cast<int>(m_number_of_faces * 3), GL_UNSIGNED_INT, 0);
        }
        else
        {
            gl.glDrawArrays(GL_POINTS, 0, static_cast<int>(m_number_of_vertices));
        }
    }
    vao->release();
}


// #endif // QGLMESH_INL