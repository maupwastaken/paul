//
// Created by Jan Schlegel on 23.07.25.
//

#pragma once

#include "Config.hpp"

#include <complex>

class Motor {
public:
    explicit Motor(pin pwm, pin dir);

    Motor() = default;

    void speed(int speed) const;

private:
    int _max = std::pow(2, PWM_RESOLUTION) - 1;

    pin _pwm;

    pin _dir;
};
