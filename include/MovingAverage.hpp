#pragma once

#include <queue>

class MovingAverage {
public:
    explicit MovingAverage(int windowSize);

    double add(double value);

private:
    int _windowSize;

    std::queue<double> _window;

    double _sum;
};
