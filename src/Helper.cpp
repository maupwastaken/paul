//
// Created by Raffael Bertram on 16.12.25.
//

#include "Helper.hpp"

RotationPID::RotationPID(double kP, double kI, double kD) {
    rotationPID = std::make_shared<PID>();
    rotationPID->setTargetPoint(0);
    rotationPID->setState(PIDState::OFF);
    rotationPID->setDirection(PIDDirection::REVERSE);
    rotationPID->setCoefficients(kP, kI, kD);
    rotationPID->setIntervalTime(5);
    rotationPID->setMode(PIDMode::CIRCULAR, -std::numbers::pi, std::numbers::pi);
    rotationPID->setOutputLimits(-20, 20);
    rotationPID->setResetIOnSetpointChange(true);
    rotationPID->setSetpointChangedThreshold(std::numbers::pi / 8);
    rotationPID->setConditionalITerm(false, 0);
}

SpeedPID::SpeedPID(double kP, double kI, double kD) {
    speedPID = std::make_shared<PID>();
    speedPID->setTargetPoint(0);
    speedPID->setState(PIDState::OFF);
    speedPID->setDirection(PIDDirection::DIRECT);
    speedPID->setCoefficients(kP, kI, kD);
    speedPID->setIntervalTime(10);
    speedPID->setMode(PIDMode::LINEAR, 0, 0);
    speedPID->setOutputLimits(-55, 55);
    speedPID->setResetIOnSetpointChange(true);
    speedPID->setSetpointChangedThreshold(5);
    speedPID->setConditionalITerm(false, 0);
}
