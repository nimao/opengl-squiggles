#include "mesh_window.h"
#include "normal.h"

#include <thread>
#include <iostream>
#include <chrono>

#include <QtGui/QGuiApplication>
#include <QtGui/QSurfaceFormat>


int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    
    QSurfaceFormat format;
    format.setSamples(128);
    format.setDepthBufferSize(16);

    std::vector<float> points;

    std::thread data_thread(
            normal_generator,
            std::ref(points),
            0.0, 10.0, 1
    );

    MeshWindow window(std::ref(points));
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
