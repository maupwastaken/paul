#include <iostream>

#include "Controller.hpp"

std::unique_ptr<Controller> bot;

void setup() {
    bot = std::make_unique<Controller>();
}

void loop() {
    bot->update();

    Vector2 driveVector;

    int factor = bot->getBallVector().getY() > 0 ? 1 : -1;

    bool goalAligned = std::abs(bot->getBallVector().getY()) < 15;

    if (bool behindBall = bot->getBallVector().getX() > 0; behindBall && goalAligned) {
        driveVector = Vector2(1, 0);
    } else {
        driveVector = Vector2::rotate(bot->getBallVector(), factor * std::numbers::pi / 2);
    }

    driveVector.normalize();

    driveVector *= 30;

    double angle = bot->getGoalVector().getAngle();

    int rotationSpeed = angle * -9;

    std::cout << "angle: " << angle << " speed: " << rotationSpeed << std::endl;

    bot->drive(driveVector);
    bot->setRotation(rotationSpeed);

    delay(5);
}
