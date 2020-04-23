#include "openglwindow.h"
#include "lattice.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <functional>

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>

float real_function(float x, float y) 
{
    //return std::sin(x * y);
    //stuff is happening in the shader atm
    Q_UNUSED(x);
    Q_UNUSED(y);

    return 0;
}

class MeshWindow : public OpenGLWindow
{
public:
    MeshWindow();

    void initialize() override;
    void render() override;

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    GLuint m_timeUniform;

    QOpenGLShaderProgram *m_program;
    int m_frame;

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;

    GLuint vao, vbo[2];
};

MeshWindow::MeshWindow()
    : m_program(0)
    , m_frame(0)
{
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    
    QSurfaceFormat format;
    format.setSamples(128);
    format.setDepthBufferSize(16);

    MeshWindow window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);


    return app.exec();
}

static const char *vertexShaderSource =
    //array([ 2.87516901, -2.12732767,  0.04421857,  0.29390206])
    //array([-0.42411458,  1.33881007,  0.01026715])
    //array([ 0.17640508, -1.71060959,  1.22561929,  0.34479632])
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "uniform highp float time;\n"
    "void main() {\n"
    "   vec4 new_pos;\n"
    "   new_pos = posAttr;\n"
    //"   new_pos.z = sin(posAttr.x * posAttr.y + 0.2*time);\n"
    "   new_pos.z = sin((posAttr.x*posAttr.x + posAttr.y*posAttr.y) + 0.2*time);\n"
    "   col = colAttr;\n"
    "   float z = 0.5 + 0.5 * new_pos.z;\n"
    "   col.x = 0.3 - 2.1*z*z + 2.9*z*z*z;\n"
    "   col.y = 1.3*z - 0.4*z*z;\n"
    "   col.z = 0.3 + 1.2*z - 1.7*z*z + 0.2*z*z*z;\n"
    "   col.w = 1.0;\n"
    "   gl_Position = matrix * new_pos;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

void MeshWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("posAttr", 0);
    m_program->bindAttributeLocation("colAttr", 1);
    m_program->link();

    m_matrixUniform = m_program->uniformLocation("matrix");
    m_timeUniform = m_program->uniformLocation("time");

    UniformRectangularLattice l(-7, 7, -7, 7, 1000, 1000);
    l.transform_mesh();
    UniformRectangularSurface s(
            real_function,
            l
    );

    colors = s.get_colors();
    vertices = s.get_vertices();
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), &colors.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void MeshWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -30);
    matrix.rotate(10.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);
    m_program->setUniformValue(m_timeUniform, (float)m_frame);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);

    m_program->release();

    ++m_frame;
}

