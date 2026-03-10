//
// Created by Raffael Bertram on 16.12.25.
//

#pragma once

#include "PID.hpp"
#include "Settings.hpp"

class RotationPID {
public:
    RotationPID(double kP, double kI, double kD);

    std::shared_ptr<PID> rotationPID;
};

class SpeedPID {
public:
    SpeedPID(double kP, double kI, double kD);

    std::shared_ptr<PID> speedPID;
};
