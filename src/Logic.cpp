//
// Created by Jan Schlegel on 01.02.26.
//

#include "Logic.hpp"

#include <iostream>

Logic::Logic() {
    worldInfo = std::make_unique<WorldInfo>(true);
    _controller = std::make_unique<Controller>();
    _rotationPID = std::make_unique<RotationPID>(ROTATION_kP, ROTATION_kI, ROTATION_kD);

    _xPID = std::make_unique<SpeedPID>(SPEED_kP, SPEED_kI, SPEED_kD);
    _yPID = std::make_unique<SpeedPID>(SPEED_kP, SPEED_kI, SPEED_kD);

    _rotationPID->rotationPID->setState(PIDState::OFF);

    _xPID->speedPID->setState(PIDState::OFF);
    _yPID->speedPID->setState(PIDState::OFF);
}

void Logic::run() {
    worldInfo->update();

    _rotationPID->rotationPID->setState(PIDState::ON);
    _xPID->speedPID->setState(PIDState::ON);
    _yPID->speedPID->setState(PIDState::ON);

    double heading = worldInfo->getCurrentHeading();

    _rotationPID->rotationPID->setTargetPoint(0);

    if (!worldInfo->isBallAligned()) {
        Vector2 relativeBallPosition = worldInfo->getBallVector().rotate(heading);
        Vector2 relativeBallTarget = relativeBallPosition - Vector2(40, 0);

        _xPID->speedPID->compute(relativeBallTarget.getX());
        _yPID->speedPID->compute(relativeBallTarget.getY());

        _driveVector = Vector2(_xPID->speedPID->getOutput(), _yPID->speedPID->getOutput() * 0.75);

        if (worldInfo->getBallVector().getMagnitude() > 60) {
            _driveVector = _driveVector.normalize() * 50;
        }
    } else if (!worldInfo->getHasBall()) {
        _driveVector = worldInfo->getBallVector().normalize();

        _driveVector *= 50;
    } else {
        _driveVector = worldInfo->getBallVector().normalize();

        _driveVector *= 25;

        heading = -worldInfo->getGoalVector().getAngle();
    }

    _rotationPID->rotationPID->compute(heading);

    _controller->setRotation(_rotationPID->rotationPID->getOutput());
    _controller->drive(_driveVector);
    _controller->updateDrive();
}


void Logic::standBy() {
    worldInfo->update();

    _rotationPID->rotationPID->setState(PIDState::OFF);
    _xPID->speedPID->setState(PIDState::OFF);
    _yPID->speedPID->setState(PIDState::OFF);

    _controller->setRotation(0);
    _controller->drive(Vector2(0, 0));
    _controller->updateDrive();
}
