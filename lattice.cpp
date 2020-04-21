#include "lattice.h"

#include <iostream>
#include <algorithm>

void UniformRectangularLattice::transform_mesh()
{
    std::vector<float> rx;
    std::vector<float> ry;

    for (size_t i = 0; i < (nx - 1); ++i) {
        for (size_t j = 0; j < ny; ++j) {
            if (i % 2 == 0) {
                rx.push_back(x.at(i * nx + j));
                ry.push_back(y.at(i * nx + j));
                rx.push_back(x.at((i+1) * nx + j));
                ry.push_back(y.at((i+1) * nx + j));
            } else {
                rx.push_back(x.at((i+1) * nx + (ny - 1 - j)));
                ry.push_back(y.at((i+1) * nx + (ny - 1 - j)));
                
                if (j == ny - 1)
                    continue;

                rx.push_back(x.at(i * nx + (ny - 2 - j)));
                ry.push_back(y.at(i * nx + (ny - 2 - j)));
            }
        }
    }

    x = rx;
    y = ry;
}

UniformRectangularLattice::UniformRectangularLattice(
        float xmin,
        float xmax,
        float ymin,
        float ymax,
        int nx,
        int ny) : nx(nx), ny(ny)
{
    float dx = (xmax - xmin) / (nx - 1);
    float dy = (ymax - ymin) / (ny - 1);

    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            x.push_back(xmin + i * dx);
            y.push_back(ymin + j * dy);
        }
    }
}

void UniformRectangularLattice::print()
{
        for (size_t i = 0; i < x.size(); ++i) {
                std::cout << x.at(i) << ' ' << y.at(i) << std::endl;
        }
}

std::vector<float> UniformRectangularSurface::get_vertices()
{
    std::vector<float> v;
    for (size_t i = 0; i < x.size(); ++i) {
        v.push_back(x.at(i));
        v.push_back(y.at(i));
        v.push_back(z.at(i));
    }

    return v;
}

std::vector<float> UniformRectangularSurface::get_colors()
{
    std::vector<float> c;
    // hardcoded rainbow 

    auto max_x = *max_element(std::begin(x), std::end(x));
    auto max_y = *max_element(std::begin(y), std::end(y));
    auto max_z = *max_element(std::begin(z), std::end(z));

    auto min_x = *min_element(std::begin(x), std::end(x));
    auto min_y = *min_element(std::begin(y), std::end(y));
    auto min_z = *min_element(std::begin(z), std::end(z));

    for (size_t i = 0; i < x.size(); ++i) {
        c.push_back((x.at(i) - min_x)/(max_x - min_x));
        c.push_back((y.at(i) - min_y)/(max_y - min_y));
        c.push_back((z.at(i) - min_z)/(max_z - min_z));
        c.push_back(1.0f);
    }

    return c;

}

void UniformRectangularSurface::print()
{
        for (size_t i = 0; i < x.size(); ++i) {
                std::cout 
                    << x.at(i) << ' ' 
                    << y.at(i) << ' '
                    << z.at(i) << std::endl;
        }
}

UniformRectangularSurface::UniformRectangularSurface(
        std::function<float(float, float)> f,
        UniformRectangularLattice lattice)
{
    x = lattice.x;
    y = lattice.y;
    for (size_t i = 0; i < x.size(); ++i) {
        z.push_back(f(x.at(i), y.at(i)));
    }
}
