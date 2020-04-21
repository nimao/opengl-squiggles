#include <functional>
#include <vector>

class UniformRectangularLattice {
private:
    float nx, ny;

public:
    std::vector<float> x;
    std::vector<float> y;

    UniformRectangularLattice(
            float xmin,
            float xmax,
            float ymin,
            float ymax,
            int nx,
            int ny);

    void transform_mesh();
    std::vector<float> get_vertices();
    void print();
};

class UniformRectangularSurface {
private:
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> z;

public:
   UniformRectangularSurface(
           std::vector<float> x,
           std::vector<float> y,
           std::vector<float> z
           ) : x(x), y(y), z(z) {}

   UniformRectangularSurface(
           std::function<float(float, float)> f,
           UniformRectangularLattice lattice);

   void print();

   std::vector<float> get_vertices();
   std::vector<float> get_colors();
};

