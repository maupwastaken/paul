#include <Arduino.h>
#include <Wire.h>
#include <iostream>

#include "CMPS14.hpp"
#include "Controller.hpp"
#include "I2CButton.hpp"
#include "PID.hpp"
#include "Helper.hpp"

std::unique_ptr<Controller> bot;
std::unique_ptr<CMPS14> cmps14;
std::unique_ptr<RotationPID> rotationPID;
std::unique_ptr<PID> yPID;

I2CButton button(0x20);

double yP = 1.35;
double yI = 0.0055;
double yD = 11;

bool botRunning = false;

void setup() {
    Wire.begin();
    Serial.begin(115200);

    cmps14 = std::make_unique<CMPS14>(0x60);
    bot = std::make_unique<Controller>();
    rotationPID = std::make_unique<RotationPID>();

    cmps14->setOrigin();
    rotationPID->rotationPID->setTargetPoint(0);
    rotationPID->rotationPID->setState(PIDState::ON);
    rotationPID->changeCoefficients(0);
    rotationPID->rotationPID->setIntervalTime(10);

    yPID = std::make_unique<PID>();
    yPID->setTargetPoint(0);
    yPID->setState(PIDState::ON);
    yPID->setDirection(PIDDirection::DIRECT);
    yPID->setCoefficients(yP, yI, yD);
    yPID->setIntervalTime(10);
    yPID->setMode(PIDMode::LINEAR, 0, 0);
    yPID->setOutputLimits(-50, 50);
    yPID->setResetIOnSetpointChange(false);
    yPID->setSetpointChangedThreshold(0.1);
    yPID->setConditionalITerm(false, 30);

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

    bool behindBall = bot->getBallVector().getX() > 5;

    bool alignedBall = std::abs(bot->getBallVector().getY()) < 15;

    double heading = cmps14->getHeadingRad();

    Vector2 driveVector;

    if (!behindBall) {
        Vector2 ballVector = bot->getBallVector();
        Vector2 ballVectorRotated = Vector2::rotate(ballVector, ballVector.getSignY() * std::numbers::pi / 2);
        ballVectorRotated.normalize();
        ballVectorRotated *= 35.0;

        driveVector = ballVector + ballVectorRotated;
        driveVector.normalize();

        driveVector *= 60;
    } else if (!alignedBall) {
        yPID->compute(bot->getBallVector().getY());

        driveVector = Vector2(0, yPID->getOutput());
    } else {
        driveVector = bot->getGoalVector();

        driveVector.normalize();

        driveVector *= 50;
    }

    rotationPID->rotationPID->compute(heading);

    bot->setRotation(rotationPID->rotationPID->getOutput());
    bot->drive(driveVector);

    delay(5);
}
