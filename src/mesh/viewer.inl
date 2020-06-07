#ifndef MESH_VIEWER_INL
#define MESH_VIEWER_INL

// #include "model.h"

// #include "mesh_data.hpp"
#include "qglmesh.hpp"
#include "viewer.hpp"

// GLM

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// QT5

#include <QMessageBox>

// STD

#include <cmath>
#include <limits>
#include <memory>

glm::vec3 bb_min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
				 std::numeric_limits<float>::max());
glm::vec3 bb_max(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(),
				 std::numeric_limits<float>::min());

void MeshViewer::init()
{
	restoreStateFromFile();
	initializeOpenGLFunctions();

	m_draw_mesh.fill(true);
	// glLineWidth(m_size_edges);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.f, 1.f);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glDisable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH_HINT);
	glBlendFunc(GL_ONE, GL_ZERO);
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);

	glLineWidth(3.0);
	glPointSize(10.0);

	std::clog << "[STATUS] OpenGL Version  : " << glGetString(GL_VERSION) << '\n';
	std::clog << "[STATUS] OpenGL Renderer : " << glGetString(GL_RENDERER) << '\n';
	std::clog << "[STATUS] OpenGL Vendor   : " << glGetString(GL_VENDOR) << '\n';
	std::clog << "[STATUS] GLSL Version    : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	//////////////////////// SHADERS //////////////////////////

	std::cerr << "[DEBUG] Vertex shader compilation...\n";
	QOpenGLShader vshader(QOpenGLShader::Vertex);
	if(!vshader.compileSourceFile("../src/model.vs"))
	{
		std::cerr << "[ERROR] compilation failure\n";
		std::cerr << vshader.log().toStdString() << '\n';
		exit(EXIT_FAILURE);
	}

	std::cerr << "[DEBUG] Fragment shader compilation...\n";
	QOpenGLShader fshader(QOpenGLShader::Fragment);
	if(!fshader.compileSourceFile("../src/model.fs"))
	{
		std::cerr << "[ERROR] compilation failure :\n";
		std::cerr << fshader.log().toStdString() << '\n';
		exit(EXIT_FAILURE);
	}

	std::cerr << "[DEBUG] Linking shader program...\n";
	mesh_shader_program.reset(new QOpenGLShaderProgram());

	if(!mesh_shader_program->addShader(&vshader))
	{
		std::cerr << "[ERROR] cannot add vertex shader to program :\n";
		std::cerr << mesh_shader_program->log().toStdString() << '\n';
		exit(EXIT_FAILURE);
	}

	if(!mesh_shader_program->addShader(&fshader))
	{
		std::cerr << "[ERROR] cannot add fragment shader to program :\n";
		std::cerr << mesh_shader_program->log().toStdString() << '\n';
		exit(EXIT_FAILURE);
	}

	if(!mesh_shader_program->link())
	{
		std::cerr << "[ERROR] Shader linking error :\n";
		std::cerr << mesh_shader_program->log().toStdString() << '\n';
		exit(EXIT_FAILURE);
	}

	this->showEntireScene();
}

void MeshViewer::add(const Mesh_data& md)
{
	// Recalcul de la boite enblobante de visualisation

	for(unsigned int i = 0; i < md.vertices.size(); i++)
	{
		bb_min = glm::min(bb_min, md.vertices[i]);
		bb_max = glm::max(bb_max, md.vertices[i]);
	}

	CGAL::qglviewer::Vec min(static_cast<qreal>(bb_min.x), static_cast<qreal>(bb_min.y),
							 static_cast<qreal>(bb_min.z));
	CGAL::qglviewer::Vec max(static_cast<qreal>(bb_max.x), static_cast<qreal>(bb_max.y),
							 static_cast<qreal>(bb_max.z));

	camera()->lookAt((min + max) / 2.0);

	setSceneBoundingBox(min, max);

	// Allocation des données sur le gpu

	meshes.emplace_back(md);
	meshes[meshes.size() - 1].use(*mesh_shader_program);
}

void MeshViewer::draw()
{
	mesh_shader_program->bind();

	glEnable(GL_DEPTH_TEST);

	GLfloat MVP_raw[16];
	this->camera()->getModelViewProjectionMatrix(MVP_raw);
	QMatrix4x4 MVP;

	for(unsigned int i = 0; i < 16; i++)
	{
		MVP.data()[i] = MVP_raw[i];
	}

	mesh_shader_program->setUniformValue("MVP", MVP);

	for(size_t i = 0; i < meshes.size(); ++i)
	{
		if(m_draw_mesh[i])
		{
			meshes[i].draw(*mesh_shader_program);
		}
	}
}

QString MeshViewer::helpString() const
{
	QString text("<h2>S e l e c t</h2>");
	text += "Left click while pressing the <b>Shift</b> key to select an object "
			"of the scene.<br><br>";
	text += "A line is drawn between the selected point and the camera selection "
			"position. ";
	text += "using <i>convertClickToLine()</i>, a useful function for analytical "
			"intersections.<br><br>";
	text += "To add object selection in your viewer, all you need to do is to "
			"define the <i>drawWithNames</i> function. ";
	text += "It gives a name to each selectable object and selection is then "
			"performed using the OpenGL <i>GL_SELECT</i> render mode.<br><br>";
	text += "Feel free to cut and paste this implementation in your own "
			"applications.";
	return text;
}

void MeshViewer::keyPressEvent(QKeyEvent* e)
{
	const ::Qt::KeyboardModifiers modifiers = e->modifiers();

	if((e->key() == ::Qt::Key_W) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[0] = !m_draw_mesh[0];
		displayMessage(QString("draw mesh[0] = %1.").arg(m_draw_mesh[0] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_X) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[1] = !m_draw_mesh[1];
		displayMessage(QString("draw mesh[1] = %1.").arg(m_draw_mesh[1] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_C) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[2] = !m_draw_mesh[2];
		displayMessage(QString("draw mesh[2] = %1.").arg(m_draw_mesh[2] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_V) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[3] = !m_draw_mesh[3];
		displayMessage(QString("draw mesh[3] = %1.").arg(m_draw_mesh[3] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_B) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[4] = !m_draw_mesh[4];
		displayMessage(QString("draw mesh[4] = %1.").arg(m_draw_mesh[4] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_N) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[5] = !m_draw_mesh[5];
		displayMessage(QString("draw mesh[5] = %1.").arg(m_draw_mesh[5] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_Comma) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[6] = !m_draw_mesh[6];
		displayMessage(QString("draw mesh[6] = %1.").arg(m_draw_mesh[6] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_Semicolon) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[7] = !m_draw_mesh[7];
		displayMessage(QString("draw mesh[7] = %1.").arg(m_draw_mesh[7] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_Colon) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[8] = !m_draw_mesh[8];
		displayMessage(QString("draw mesh[8] = %1.").arg(m_draw_mesh[8] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_Exclam) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[9] = !m_draw_mesh[9];
		displayMessage(QString("draw mesh[9] = %1.").arg(m_draw_mesh[9] ? "true" : "false"));
		update();
	}
	else
	{
		CGAL::QGLViewer::keyPressEvent(e);
	}
}

// bool MeshViewer::GLLogErrors()
// {
// 	GLenum error = GL_NO_ERROR;

// 	while((error = glGetError()) != GL_NO_ERROR)
// 	{
// 		std::cerr << "[ERROR] OpenGL : " << gluErrorString(error) << '\n';
// 		return true;
// 	}
// 	return false;
// }

#endif // MESH_VIEWER_INL