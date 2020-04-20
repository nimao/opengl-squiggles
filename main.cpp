#include "openglwindow.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>


std::vector<GLfloat> read_floats(const std::string&);

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
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
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

    vertices = read_floats("strip.csv");
    colors = read_floats("col.csv");
    
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
    matrix.translate(0, 0, -10);
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);

    m_program->release();

    ++m_frame;
}

std::vector<GLfloat> read_floats(const std::string& filename)
{
    std::ifstream file(filename);
    std::string line;

    std::vector<GLfloat> vertices;

    while (std::getline(file, line))
    {
        std::stringstream lineStream(line);
        std::string raw_field;

        while (std::getline(lineStream, raw_field, ','))
            vertices.push_back(std::stof(raw_field));
    }

    return vertices;
}
