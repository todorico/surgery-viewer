// #ifndef MESH_VIEWER_INL
// #define MESH_VIEWER_INL

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

// CGAL::qglviewer::Vec bb_min(std::numeric_limits<qreal>::max(),

glm::vec3 bb_min(std::numeric_limits<float>::max(),
				 std::numeric_limits<float>::max(),
				 std::numeric_limits<float>::max());
glm::vec3 bb_max(std::numeric_limits<float>::min(),
				 std::numeric_limits<float>::min(),
				 std::numeric_limits<float>::min());

/*MeshViewer::MeshViewer()
{


	defaultConstructor();

		QSurfaceFormat format;
	format.setVersion(3, 0);
	format.setProfile(QSurfaceFormat::CoreProfile); // Requires >=Qt-4.8.0
	setFormat(format);
}*/

bool MeshViewer::GLLogErrors()
{
	GLenum error = GL_NO_ERROR;

	while((error = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "[ERROR] OpenGL : ";
		switch(error)
		{
			case GL_INVALID_ENUM:
				std::cerr << "GL_INVALID_ENUM\n";
				break;
			case GL_INVALID_VALUE:
				std::cerr << "GL_INVALID_VALUE\n";
				break;
			case GL_INVALID_OPERATION:
				std::cerr << "GL_INVALID_OPERATION\n";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
				break;
			case GL_OUT_OF_MEMORY:
				std::cerr << "GL_OUT_OF_MEMORY\n";
				break;
			case GL_STACK_UNDERFLOW:
				std::cerr << "GL_STACK_UNDERFLOW\n";
				break;
			case GL_STACK_OVERFLOW:
				std::cerr << "GL_STACK_OVERFLOW\n";
				break;
			default:
				std::cerr << "GL_UNKNOWN_ERROR\n";
				break;
		}
		return true;
	}
	return false;
}

void MeshViewer::initShaders()
{
	QString app_dir = QCoreApplication::applicationDirPath();
	//////////// SHADER_PROGRAM : COLOR_ONLY

	{
		std::cerr << "[DEBUG] Vertex shader compilation...\n";
		QOpenGLShader vshader(QOpenGLShader::Vertex);
		if(!vshader.compileSourceFile(app_dir + "/shader/vertex.vert"))
		{
			std::cerr << "[ERROR] compilation failure\n";
			std::cerr << vshader.log().toStdString() << '\n';
			exit(EXIT_FAILURE);
		}

		std::cerr << "[DEBUG] Fragment shader compilation...\n";
		QOpenGLShader fshader(QOpenGLShader::Fragment);
		if(!fshader.compileSourceFile(app_dir +
									  "/shader/fragment_color_only.frag"))
		{
			std::cerr << "[ERROR] compilation failure :\n";
			std::cerr << fshader.log().toStdString() << '\n';
			exit(EXIT_FAILURE);
		}

		std::cerr << "[DEBUG] Linking shader program...\n";
		shader_program_color_only.reset(new QOpenGLShaderProgram());

		if(!shader_program_color_only->addShader(&vshader))
		{
			std::cerr << "[ERROR] cannot add vertex shader to program :\n";
			std::cerr << shader_program_color_only->log().toStdString() << '\n';
			exit(EXIT_FAILURE);
		}

		if(!shader_program_color_only->addShader(&fshader))
		{
			std::cerr << "[ERROR] cannot add fragment shader to program :\n";
			std::cerr << shader_program_color_only->log().toStdString() << '\n';
			exit(EXIT_FAILURE);
		}

		if(!shader_program_color_only->link())
		{
			std::cerr << "[ERROR] Shader linking error :\n";
			std::cerr << shader_program_color_only->log().toStdString() << '\n';
			exit(EXIT_FAILURE);
		}
	}

	//////////// SHADER_PROGRAM : TEXTURE_ONLY

	{
		std::cerr << "[DEBUG] Vertex shader compilation...\n";
		QOpenGLShader vshader(QOpenGLShader::Vertex);
		if(!vshader.compileSourceFile(app_dir + "/shader/vertex.vert"))
		{
			std::cerr << "[ERROR] compilation failure\n";
			std::cerr << vshader.log().toStdString() << '\n';
			exit(EXIT_FAILURE);
		}

		std::cerr << "[DEBUG] Fragment shader compilation...\n";
		QOpenGLShader fshader(QOpenGLShader::Fragment);
		if(!fshader.compileSourceFile(app_dir +
									  "/shader/fragment_texture_only.frag"))
		{
			std::cerr << "[ERROR] compilation failure :\n";
			std::cerr << fshader.log().toStdString() << '\n';
			exit(EXIT_FAILURE);
		}

		std::cerr << "[DEBUG] Linking shader program...\n";
		shader_program_texture_only.reset(new QOpenGLShaderProgram());

		if(!shader_program_texture_only->addShader(&vshader))
		{
			std::cerr << "[ERROR] cannot add vertex shader to program :\n";
			std::cerr << shader_program_texture_only->log().toStdString()
					  << '\n';
			exit(EXIT_FAILURE);
		}

		if(!shader_program_texture_only->addShader(&fshader))
		{
			std::cerr << "[ERROR] cannot add fragment shader to program :\n";
			std::cerr << shader_program_texture_only->log().toStdString()
					  << '\n';
			exit(EXIT_FAILURE);
		}

		if(!shader_program_texture_only->link())
		{
			std::cerr << "[ERROR] Shader linking error :\n";
			std::cerr << shader_program_texture_only->log().toStdString()
					  << '\n';
			exit(EXIT_FAILURE);
		}
	}

	//////////// SHADER_PROGRAM : COLOR_AND_TEXTURE

	{
		std::cerr << "[DEBUG] Vertex shader compilation...\n";
		QOpenGLShader vshader(QOpenGLShader::Vertex);
		if(!vshader.compileSourceFile(app_dir + "/shader/vertex.vert"))
		{
			std::cerr << "[ERROR] compilation failure\n";
			std::cerr << vshader.log().toStdString() << '\n';
			exit(EXIT_FAILURE);
		}

		std::cerr << "[DEBUG] Fragment shader compilation...\n";
		QOpenGLShader fshader(QOpenGLShader::Fragment);
		if(!fshader.compileSourceFile(
			   app_dir + "/shader/fragment_color_and_texture.frag"))
		{
			std::cerr << "[ERROR] compilation failure :\n";
			std::cerr << fshader.log().toStdString() << '\n';
			exit(EXIT_FAILURE);
		}

		std::cerr << "[DEBUG] Linking shader program...\n";
		shader_program_color_and_texture.reset(new QOpenGLShaderProgram());

		if(!shader_program_color_and_texture->addShader(&vshader))
		{
			std::cerr << "[ERROR] cannot add vertex shader to program :\n";
			std::cerr << shader_program_color_and_texture->log().toStdString()
					  << '\n';
			exit(EXIT_FAILURE);
		}

		if(!shader_program_color_and_texture->addShader(&fshader))
		{
			std::cerr << "[ERROR] cannot add fragment shader to program :\n";
			std::cerr << shader_program_color_and_texture->log().toStdString()
					  << '\n';
			exit(EXIT_FAILURE);
		}

		if(!shader_program_color_and_texture->link())
		{
			std::cerr << "[ERROR] Shader linking error :\n";
			std::cerr << shader_program_color_and_texture->log().toStdString()
					  << '\n';
			exit(EXIT_FAILURE);
		}
	}
}

void MeshViewer::init()
{
	// setSurfaceType(QWindow::OpenGLSurface);
	restoreStateFromFile();
	// std::cerr << "viewer context valid? : \n" << this->context()->isValid()
	// << '\n';
	std::cerr << "viewer context adress : " << this->context() << '\n';

	// initializeOpenGLFunctions();

	m_draw_mesh.fill(true);
	// glLineWidth(m_size_edges);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.f, 1.f);
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);
	glDisable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	// glDisable(GL_POLYGON_SMOOTH_HINT);
	glBlendFunc(GL_ONE, GL_ZERO);
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);

	glLineWidth(2.0);
	glPointSize(10.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	std::clog << "[STATUS] OpenGL Version  : " << glGetString(GL_VERSION)
			  << '\n';
	std::clog << "[STATUS] OpenGL Renderer : " << glGetString(GL_RENDERER)
			  << '\n';
	std::clog << "[STATUS] OpenGL Vendor   : " << glGetString(GL_VENDOR)
			  << '\n';
	std::clog << "[STATUS] GLSL Version    : "
			  << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

	//////////////////////// SHADERS //////////////////////////

	this->initShaders();

	this->showEntireScene();

	// std::cerr << "viewer context valid? : " << this->context().isValid() <<
	// '\n';
	std::cerr << "viewer context adress : " << this->context() << '\n';
}

void MeshViewer::add(const Mesh_data& md)
{
	if(!md.positions.has_value())
	{
		std::cerr
			<< "[WARNING] cannot view mesh data without positions defined\n";
		return;
	}

	// Re-calcul de la boite enblobante de visualisation

	for(unsigned int i = 0; i < md.positions->size(); i++)
	{
		auto position = glm::vec3((*md.positions)[i][0], (*md.positions)[i][1],
								  (*md.positions)[i][2]);

		bb_min = glm::min(bb_min, position);
		bb_max = glm::max(bb_max, position);
	}

	CGAL::qglviewer::Vec min(static_cast<qreal>(bb_min.x),
							 static_cast<qreal>(bb_min.y),
							 static_cast<qreal>(bb_min.z));
	CGAL::qglviewer::Vec max(static_cast<qreal>(bb_max.x),
							 static_cast<qreal>(bb_max.y),
							 static_cast<qreal>(bb_max.z));

	camera()->lookAt((min + max) / 2.0);

	setSceneBoundingBox(min, max);

	// Allocation des données sur le gpu

	if(md.positions.has_value() && md.texture_path.has_value())
	{
		std::cerr << "[DEBUG] Using color and texture shader\n";
		used_shader_program = shader_program_color_and_texture.get();
	}
	else if(md.texture_path.has_value())
	{
		std::cerr << "[DEBUG] Using texture only shader\n";
		used_shader_program = shader_program_texture_only.get();
	}
	else
	{
		std::cerr << "[DEBUG] Using color only shader\n";
		used_shader_program = shader_program_color_only.get();
	}

	meshes.emplace_back(md);

	meshes[meshes.size() - 1].use(*used_shader_program);
	// meshes[meshes.size() - 1].use(*shader_program_texture_only);
}

void MeshViewer::draw()
{
	if(used_shader_program)
	{
		glEnable(GL_DEPTH_TEST);

		GLfloat MVP_matrix_raw[16];
		this->camera()->getModelViewProjectionMatrix(MVP_matrix_raw);
		QMatrix4x4 MVP_matrix;

		for(unsigned int i = 0; i < 16; i++)
		{
			MVP_matrix.data()[i] = MVP_matrix_raw[i];
		}

		used_shader_program->bind();
		used_shader_program->setUniformValue("MVP_matrix", MVP_matrix);

		GLfloat V_matrix_raw[16];
		this->camera()->getModelViewMatrix(V_matrix_raw);
		QMatrix4x4 V_matrix;

		for(unsigned int i = 0; i < 16; i++)
		{
			V_matrix.data()[i] = V_matrix_raw[i];
		}

		QVector3D camera_position(this->camera()->position()[0],
								  this->camera()->position()[1],
								  this->camera()->position()[2]);

		QVector3D camera_direction(this->camera()->viewDirection()[0],
								   this->camera()->viewDirection()[1],
								   this->camera()->viewDirection()[2]);

		used_shader_program->setUniformValue("V_matrix", V_matrix);
		used_shader_program->setUniformValue("camera_position",
											 camera_position);
		used_shader_program->setUniformValue("camera_direction",
											 camera_direction);

		if(m_draw_triangles)
		{
			if(m_draw_edges)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			for(size_t i = 0; i < meshes.size(); ++i)
			{
				if(m_draw_mesh[i])
				{
					meshes[i].draw(*this, *used_shader_program, GL_TRIANGLES);
				}
			}

			if(m_draw_edges)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if(m_draw_points)
		{
			for(size_t i = 0; i < meshes.size(); ++i)
			{
				if(m_draw_mesh[i])
				{
					meshes[i].draw(*this, *used_shader_program, GL_POINTS);
				}
			}
		}
	}
}

QString MeshViewer::helpString() const
{
	QString text("<h2>S e l e c t</h2>");
	text +=
		"Left click while pressing the <b>Shift</b> key to select an object "
		"of the scene.<br><br>";
	text +=
		"A line is drawn between the selected point and the camera selection "
		"position. ";
	text +=
		"using <i>convertClickToLine()</i>, a useful function for analytical "
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
		displayMessage(QString("draw mesh[0] = %1.")
						   .arg(m_draw_mesh[0] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_X) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[1] = !m_draw_mesh[1];
		displayMessage(QString("draw mesh[1] = %1.")
						   .arg(m_draw_mesh[1] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_C) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[2] = !m_draw_mesh[2];
		displayMessage(QString("draw mesh[2] = %1.")
						   .arg(m_draw_mesh[2] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_V) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[3] = !m_draw_mesh[3];
		displayMessage(QString("draw mesh[3] = %1.")
						   .arg(m_draw_mesh[3] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_B) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[4] = !m_draw_mesh[4];
		displayMessage(QString("draw mesh[4] = %1.")
						   .arg(m_draw_mesh[4] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_N) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[5] = !m_draw_mesh[5];
		displayMessage(QString("draw mesh[5] = %1.")
						   .arg(m_draw_mesh[5] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_Q) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[6] = !m_draw_mesh[6];
		displayMessage(QString("draw mesh[6] = %1.")
						   .arg(m_draw_mesh[6] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_S) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[7] = !m_draw_mesh[7];
		displayMessage(QString("draw mesh[7] = %1.")
						   .arg(m_draw_mesh[7] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_D) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[8] = !m_draw_mesh[8];
		displayMessage(QString("draw mesh[8] = %1.")
						   .arg(m_draw_mesh[8] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_F) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[9] = !m_draw_mesh[9];
		displayMessage(QString("draw mesh[9] = %1.")
						   .arg(m_draw_mesh[9] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_G) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[10] = !m_draw_mesh[10];
		displayMessage(QString("draw mesh[10] = %1.")
						   .arg(m_draw_mesh[10] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_H) && (modifiers == ::Qt::NoButton))
	{
		m_draw_mesh[11] = !m_draw_mesh[11];
		displayMessage(QString("draw mesh[11] = %1.")
						   .arg(m_draw_mesh[11] ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_T) && (modifiers == ::Qt::NoButton))
	{
		m_draw_triangles = !m_draw_triangles;
		displayMessage(QString("draw triangles = %1.")
						   .arg(m_draw_triangles ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_E) && (modifiers == ::Qt::NoButton))
	{
		m_draw_edges = !m_draw_edges;
		displayMessage(
			QString("draw edges = %1.").arg(m_draw_edges ? "true" : "false"));
		update();
	}
	else if((e->key() == ::Qt::Key_P) && (modifiers == ::Qt::NoButton))
	{
		m_draw_points = !m_draw_points;
		displayMessage(
			QString("draw points = %1.").arg(m_draw_points ? "true" : "false"));
		update();
	}
	else
	{
		CGAL::QGLViewer::keyPressEvent(e);
	}
}

// #endif // MESH_VIEWER_INL
