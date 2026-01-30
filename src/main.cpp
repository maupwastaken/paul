#include <Arduino.h>
#include <Wire.h>
#include <iostream>

#include "CMPS14.hpp"
#include "Controller.hpp"
#include "I2CButton.hpp"
#include "MovingAverage.hpp"

std::unique_ptr<Controller> bot;
std::unique_ptr<CMPS14> cmps14;

MovingAverage alignedAvg(10);
MovingAverage behindBallAvg(5);

I2CButton button(0x20);

bool botRunning = false;

double getSpeed(double const x) {
    return 3.75 * std::pow(1.45833, 15.0 * x - 8.0) + 25.0;
}

void setup() {
    Wire.begin();
    Serial.begin(115200);

    cmps14 = std::make_unique<CMPS14>(0x60);
    bot = std::make_unique<Controller>();

    cmps14->setOrigin();

    button.setColor(ButtonSide::LEFT, Color::BLUE);
    button.setColor(ButtonSide::RIGHT, Color::MAGENTA);
}

void loop() {
    cmps14->update();
    bot->update();
    button.update();

    if (button.getButtonStateOnce(ButtonSide::LEFT)) {
        botRunning = !botRunning;
    }
    if (button.getButtonStateOnce(ButtonSide::RIGHT)) {
        cmps14->setOrigin();
    }

    if (!botRunning) {
        button.setColor(ButtonSide::LEFT, Color::BLUE);
        button.setColor(ButtonSide::RIGHT, Color::MAGENTA);

        bot->drive(Vector2(0, 0));
        bot->setRotation(0);

        return;
    }

    button.setColor(ButtonSide::LEFT, Color::OFF);
    button.setColor(ButtonSide::RIGHT, Color::OFF);

    Vector2 driveVector;

    double driveSpeed;

    bool ballAligned = alignedAvg.add(std::abs(bot->getBallVector().getY()) < 20) > 0;

    bool behindBall = behindBallAvg.add(bot->getBallVector().getX() > 0) > 0;

    Vector2 ballVectorSpeed = bot->getBallVector();

    ballVectorSpeed.normalize();

    if (!behindBall) {
        double vx = std::abs(ballVectorSpeed.getX());
        double vy = std::abs(ballVectorSpeed.getY());

        double driveComponent = vx * vx * vx + vy * vy;

        driveSpeed = getSpeed(driveComponent);
    } else {
        driveSpeed = 50;
    }

    driveSpeed = std::clamp(driveSpeed, 20.0, 80.0);

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
        rotationSpeedFactor = 9;
    } else {
        Vector2 ballVector = bot->getBallVector();
        Vector2 ballVectorRotated = Vector2::rotate(ballVector, ballVector.getSignY() * std::numbers::pi / 2);
        ballVectorRotated.normalize();
        ballVectorRotated *= 35.0;

        driveVector = ballVector + ballVectorRotated;
        driveVector.normalize();

        heading = cmps14->getHeadingRad();
        rotationSpeedFactor = 9;
    }

    driveVector *= driveSpeed;

    bot->drive(driveVector);
    bot->setRotation(-heading * rotationSpeedFactor);
    delay(5);
}


//test git