#pragma once

#include "Motor.hpp"
#include "Settings.hpp"
#include "Vector2.hpp"

class Controller {
public:
    explicit Controller();

    void motor(int motor, int speed) const;

    void drive(const Vector2 &v);

    void setRotation(double ω);

    void updateDrive();

private:
    std::array<Motor, PWM_PINS.size()> _motors{};

    Vector2 _driveVector;

    double _rotationSpeed;
};
