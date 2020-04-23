#include "openglwindow.h"

#include <vector>

#include <QtGui/QOpenGLShaderProgram>
#include <QtCore/qmath.h>

class MeshWindow : public OpenGLWindow
{
public:
    MeshWindow(std::vector<float>& points);
    MeshWindow() = delete;

    void initialize() override;
    void render() override;

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    GLuint m_timeUniform;

    GLuint vao, vbo[2];
    GLuint vao2, vbo2[1];

    int m_frame;

    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *shader2;
    QOpenGLShaderProgram *shader3;

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    const std::vector<float>& points;
};
