#include "Controller.hpp"

#include <Arduino.h>
#include <iostream>
#include <Wire.h>
#include <utility>

Controller::Controller() {
    for (int i = 0; i < _motors.size(); i++) {
        _motors[i] = Motor(PWM_PINS[i], DIR_PINS[i]);
    }

    pinMode(ENA, OUTPUT);
    digitalWrite(ENA, LOW);

    _rotationSpeed = 0;
}

void Controller::motor(int motor, int speed) const {
    if (motor < 0 || motor > 3) {
        std::cerr << "Motor out of range!" << std::endl;
    }

    std::cout << speed << std::endl;

    _motors[motor].speed(speed);
}

void Controller::drive(const Vector2 &v) {
    _driveVector = v.clone();
}

void Controller::setRotation(double ω) {
    _rotationSpeed = ω;
}

void Controller::updateDrive() {
    using enum MOTOR_CONFIGURATION;

    std::array<int, 3> motorSpeeds{};

    auto wSpeed = static_cast<int>(_rotationSpeed);

    double x = _driveVector.getX();
    double y = _driveVector.getY();

    double xAngle = sqrt(3) / 2; // cos30
    double yAngle = 0.5;

    motorSpeeds[std::to_underlying(RIGHT)] = static_cast<int>(xAngle * x + yAngle * y);
    motorSpeeds[std::to_underlying(LEFT)] = static_cast<int>(-xAngle * x + yAngle * y);
    motorSpeeds[std::to_underlying(BACK)] = static_cast<int>(-y);

    int maxSpeed = 0;

    for (const int motorSpeed: motorSpeeds) {
        maxSpeed = std::max(maxSpeed, std::abs(motorSpeed + wSpeed));
    }

    if (maxSpeed > 100) {
        for (int &motorSpeed: motorSpeeds) {
            motorSpeed = static_cast<int>(std::round(static_cast<double>(motorSpeed) / static_cast<double>(maxSpeed)));
        }
        wSpeed = static_cast<int>(std::round(static_cast<double>(wSpeed) / static_cast<double>(maxSpeed)));
    }

    for (size_t i = 0; i < motorSpeeds.size(); ++i) {
        _motors[i].speed(motorSpeeds[i] + wSpeed);
    }
}
