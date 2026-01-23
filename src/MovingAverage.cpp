//
// Created by Jan Schlegel on 19.01.26.
//

#include "MovingAverage.hpp"

MovingAverage::MovingAverage(int windowSize) {
    _windowSize = windowSize;
    _sum = 0.0;
}

double MovingAverage::add(double value) {
    _sum += value;
    _window.push(value);

    if (_window.size() > _windowSize) {
        _sum -= _window.front();
        _window.pop();
    }

    return _sum / _window.size();
}
