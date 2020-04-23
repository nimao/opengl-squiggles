#include "normal.h"

#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <vector>

void normal_generator(std::vector<float>& v, double mu, double sigma, int t)
{
    std::default_random_engine generator;
    std::normal_distribution<double> dist(mu, sigma);

    for (;;)
    {
        float x = dist(generator);

        //std::cout << x << std::endl;
        v.push_back(x);
        std::this_thread::sleep_for(std::chrono::milliseconds(t));


    }

}
