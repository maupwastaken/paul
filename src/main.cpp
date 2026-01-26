#include <Arduino.h>
#include <Wire.h>
#include <iostream>

#include "CMPS14.hpp"
#include "Controller.hpp"
#include "MovingAverage.hpp"

std::unique_ptr<Controller> bot;
std::unique_ptr<CMPS14> cmps14;

MovingAverage speedAvg(10);
MovingAverage alignedAvg(8);

double getSpeed(double distance) {
    return 0.6 * distance + 20;
}

void setup() {
    Wire.begin();
    Serial.begin(115200);

    cmps14 = std::make_unique<CMPS14>(0x60);
    bot = std::make_unique<Controller>();

    cmps14->setOrigin();
}

void loop() {
    cmps14->update();
    bot->update();

    Vector2 driveVector;

    double driveSpeed;

    bool ballAligned = alignedAvg.add(std::abs(bot->getBallVector().getY()) < 15) > 0;

    bool behindBall = bot->getBallVector().getX() > 0;

    if (!behindBall) {
        driveSpeed = 40;
    } else if (!ballAligned) {
        driveSpeed = getSpeed(std::abs(bot->getBallVector().getY()));
    } else {
        driveSpeed = 45;
    }

    double newDriveSpeed = speedAvg.add(driveSpeed);

    double rotationSpeedFactor = 0.0;

    double heading = 0.0;

    if (bot->hasBall()) {
        driveVector = bot->getGoalVector();
        driveVector.normalize();

        heading = -bot->getGoalVector().getAngle();
        rotationSpeedFactor = 8;
    } else if (behindBall && ballAligned && !bot->hasBall()) {
        driveVector = bot->getBallVector();
        driveVector.normalize();

        heading = cmps14->getHeadingRad();
        rotationSpeedFactor = 8;
    } else {
        Vector2 ballVector = bot->getBallVector();
        Vector2 ballVectorRotated = Vector2::rotate(ballVector, ballVector.getSignY() * std::numbers::pi / 2);
        ballVectorRotated.normalize();
        ballVectorRotated *= 40.0;

        driveVector = ballVector + ballVectorRotated;
        driveVector.normalize();

        heading = cmps14->getHeadingRad();
        rotationSpeedFactor = 9;
    }

    driveVector *= newDriveSpeed;

    bot->drive(driveVector);
    bot->setRotation(-heading * rotationSpeedFactor);


    delay(5);
}
