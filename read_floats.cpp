#include <vector>
#include <sstream>
#include <fstream>

std::vector<float> read_floats(const std::string& filename)
{
    std::ifstream file(filename);
    std::string line;

    std::vector<float> vertices;

    while (std::getline(file, line))
    {
        std::stringstream lineStream(line);
        std::string raw_field;

        while (std::getline(lineStream, raw_field, ','))
            vertices.push_back(std::stof(raw_field));
    }

    return vertices;
}
