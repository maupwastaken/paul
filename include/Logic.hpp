//
// Created by Jan Schlegel on 01.02.26.
//

#pragma once

#include "Controller.hpp"
#include "WorldInfo.hpp"
#include "Helper.hpp"

class Logic {
public:
    Logic();

    void run();

    void standBy();

    std::unique_ptr<WorldInfo> worldInfo;

private:
    void driveBehindBall();

    void driveToBall();

    void driveToGoal();

    double _currentHeading;

    double _driveSpeed;

    Vector2 _driveVector;

    std::unique_ptr<Controller> _controller;

    std::unique_ptr<RotationPID> _rotationPID;

    std::unique_ptr<SpeedPID> _xPID;
    std::unique_ptr<SpeedPID> _yPID;
};
