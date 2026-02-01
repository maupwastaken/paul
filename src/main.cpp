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
MovingAverage behindBallAvg(10);

I2CButton button(0x20);

bool botRunning = false;
bool hasBall = false;
bool alreadyAligned = false;

std::chrono::time_point<std::chrono::high_resolution_clock> lastBallAlignedTimePoint;

std::chrono::duration<std::chrono::milliseconds> ballAlignedTime();

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

    double driveSpeed = 50.0;

    bool ballAligned = alignedAvg.add(std::abs(bot->getBallVector().getY()) < 15 && bot->getBallVector().getX() > 5) > 0;

    double behindBallAvgValue = behindBallAvg.add(bot->getBallVector().getX() > 0);
    bool behindBall = behindBallAvgValue > 0.6;

    Vector2 ballVectorSpeed = bot->getBallVector();

    ballVectorSpeed.normalize();

    double vx = std::abs(ballVectorSpeed.getX());
    double vy = std::abs(ballVectorSpeed.getY());

    if (ballAligned) {
        if (!alreadyAligned) {
            lastBallAlignedTimePoint = std::chrono::high_resolution_clock::now();
            alreadyAligned = true;
        }

        if (std::chrono::high_resolution_clock::now() - lastBallAlignedTimePoint > std::chrono::seconds(3)) {
            hasBall = true;
        }
    } else {
        alreadyAligned = false;
        hasBall = false;
    }

    double driveComponent = vx * vx + vy * vy * vy;

    double targetSpeed = !behindBall ? getSpeed(driveComponent) * 0.9 : std::clamp(65 * vy, 35.0, 65.0);

    driveSpeed = 0.9 * driveSpeed + 0.1 * targetSpeed;

    driveSpeed = std::clamp(driveSpeed, 35.0, 80.0);

    double rotationSpeedFactor = 0.0;

    double heading = 0.0;

    if (hasBall) {
        driveVector = bot->getGoalVector();
        driveVector.normalize();

        heading = bot->getGoalVector().getAngle();
        rotationSpeedFactor = 8;
    } else if (behindBall && ballAligned && !hasBall) {
        driveVector = bot->getBallVector();
        driveVector.normalize();

        heading = -cmps14->getHeadingRad();
        rotationSpeedFactor = 9;
    } else {
        Vector2 ballVector = bot->getBallVector();
        Vector2 ballVectorRotated = Vector2::rotate(ballVector, ballVector.getSignY() * std::numbers::pi / 2);
        ballVectorRotated.normalize();
        ballVectorRotated *= 35.0;

        driveVector = ballVector + ballVectorRotated;
        driveVector.normalize();

        heading = -cmps14->getHeadingRad();
        rotationSpeedFactor = 9;
    }

    std::cout << hasBall << " " << ballAligned << " " << alreadyAligned << std::endl;

    driveVector *= driveSpeed;

    bot->drive(driveVector);
    bot->setRotation(heading * rotationSpeedFactor);

    delay(5);
}
