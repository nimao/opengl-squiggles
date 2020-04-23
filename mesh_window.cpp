#include "mesh_window.h"

#include "lattice.h"
#include "normal.h"

#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>

MeshWindow::MeshWindow(std::vector<float>& _points)
    : m_frame(0)
    , m_program(0)
    , shader2(0)
    , shader3(0)
    , points(_points)
{
}

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "uniform highp float time;\n"
    "void main() {\n"
    "   vec4 new_pos;\n"
    "   new_pos = posAttr;\n"
    "   new_pos.z = sin((posAttr.x*posAttr.x + posAttr.y*posAttr.y) + 0.2*time);\n"
    "   col = colAttr;\n"
    "   float z = 0.5 + 0.5 * new_pos.z;\n"
    "   col.x = 0.3 - 2.1*z*z + 2.9*z*z*z;\n"
    "   col.y = 1.3*z - 0.4*z*z;\n"
    "   col.z = 0.3 + 1.2*z - 1.7*z*z + 0.2*z*z*z;\n"
    "   col.w = 1.0;\n"
    "   gl_Position = matrix * new_pos;\n"
    "}\n";

static const char *vertexShaderSource2 =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "uniform highp float time;\n"
    "void main() {\n"
    "   vec4 new_pos;\n"
    "   new_pos = posAttr;\n"
    "   new_pos.z = 3.0 + sin((posAttr.x*posAttr.x + posAttr.y*posAttr.y) + 0.2*time);\n"
    "   col = colAttr;\n"
    "   float z = 0.5 + 0.5 * (new_pos.z - 3.0);\n"
    "   col.x = 0.3 - 2.1*z*z + 2.9*z*z*z;\n"
    "   col.y = 1.3*z - 0.4*z*z;\n"
    "   col.z = 0.3 + 1.2*z - 1.7*z*z + 0.2*z*z*z;\n"
    "   col.w = 1.0;\n"
    "   gl_Position = matrix * new_pos;\n"
    "}\n";

static const char *vertexShaderSource3 =
    "attribute highp vec4 posAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col.xyz = posAttr.xyz;\n"
    "   col.w = 1.0;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

float real_function(float x, float y) 
{
    Q_UNUSED(x);
    Q_UNUSED(y);

    return 0;
}

void MeshWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

    m_program->bindAttributeLocation("posAttr", 0);
    m_program->bindAttributeLocation("colAttr", 1);

    m_program->link();

    
    shader2 = new QOpenGLShaderProgram(this);
    shader2->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource2);
    shader2->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

    shader2->bindAttributeLocation("posAttr", 0);
    shader2->bindAttributeLocation("colAttr", 1);

    shader2->link();

    shader3 = new QOpenGLShaderProgram(this);
    shader3->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource3);
    shader3->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

    shader3->bindAttributeLocation("posAttr", 0);
    shader3->link();

    m_matrixUniform = m_program->uniformLocation("matrix");
    m_timeUniform = m_program->uniformLocation("time");

    UniformRectangularLattice l(-7, 7, -7, 7, 1000, 1000);
    l.transform_mesh();
    UniformRectangularSurface s(
            real_function,
            l
    );

    vertices = s.get_vertices();
    colors = s.get_colors();

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

    glGenVertexArrays(1, &vao2);

    /*
    glBindVertexArray(vao2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
    glBufferData(GL_ARRAY_BUFFER, 1024000, &points.front(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    */


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
    glBindVertexArray(0);

    m_program->release();

    
    shader2->bind();

    shader2->setUniformValue(
            shader2->uniformLocation("matrix"),
            matrix
    );
    shader2->setUniformValue(
            shader2->uniformLocation("time"),
            (float)m_frame
    );

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);
    glBindVertexArray(0);

    shader2->release();
    
    
    shader3->bind();
    shader3->setUniformValue(
            shader3->uniformLocation("matrix"),
            matrix
    );

    glBindVertexArray(vao2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
    glBufferData(GL_ARRAY_BUFFER, 1024000, &points.front(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, points.size() / 3);
    glBindVertexArray(0);

    shader3->release();

    ++m_frame;
}
