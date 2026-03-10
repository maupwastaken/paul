//
// Created by Jan Schlegel on 01.02.26.
//

#include "Logic.hpp"

#include <iostream>

Logic::Logic() {
    worldInfo = std::make_unique<WorldInfo>(false);
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

    double speed = 40.0;
    double heading = 0.0;

    if (worldInfo->getHasBall()) {
        if (worldInfo->getGoalExists()) {
            _driveVector = worldInfo->getGoalVector();
        } else {
            _driveVector = Vector2(0, 1 * worldInfo->getGoalVector().getAngle()).normalize();

            std::cout << _driveVector << std::endl;
        }
        heading = worldInfo->getCurrentHeading();
    } else if (worldInfo->botBehindBall() && worldInfo->isBallAligned()) {
        _driveVector = worldInfo->getBallVector();

        heading = worldInfo->getCurrentHeading();
    } else {
        Vector2 ballVectorRotated = worldInfo->getBallVector().rotate(worldInfo->getBallVector().getSignY() * std::numbers::pi / 2);

        ballVectorRotated.normalize();
        ballVectorRotated *= 35.0;

        _driveVector = worldInfo->getBallVector() + ballVectorRotated;

        heading = worldInfo->getCurrentHeading();
    }

    _rotationPID->rotationPID->compute(heading);

    _controller->setRotation(_rotationPID->rotationPID->getOutput());
    _controller->drive(_driveVector.normalize() * speed);
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
