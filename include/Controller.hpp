#pragma once

#include <Pixy2I2C.h>

#include "Motor.hpp"
#include "Vector2.hpp"

class Controller {
public:
    explicit Controller();

    void motor(int motor, int speed);

    void drive(const Vector2 &v);

    void setRotation(double ω);

    template <typename D>
    void kick(std::chrono::duration<D> duration = KICK_STRENGTH);

    void update();

    Vector2 getBallVector();

    Vector2 getGoalVector();

    double getGoalWidth();

    bool hasBall();

private:
    void updateDrive();

    void updateIRData();

    void updatePixyData();

    std::array<Motor, 4> _motors{};

    Vector2 _driveVector;

    Vector2 _ballVector;
    Vector2 _goalVector;

    double _goalWidth;

    double _rotationSpeed;

    Pixy2I2C _pixy;

    std::chrono::time_point<std::chrono::high_resolution_clock> _lastKick;
};
