//
// Created by Jan Schlegel on 23.07.25.
//

#include "Controller.hpp"

#include <Arduino.h>
#include <iostream>
#include <Wire.h>
#include <thread>
#include <utility>

Controller::Controller() {
    Serial.begin(115200);

    _pixy.init(PIXY_ADDRESS);

    for (int i = 0; i < 4; i++) {
        _motors[i] = Motor(PWM_PINS[i], DIR_PINS[i]);
    }

    pinMode(ENA, OUTPUT);
    digitalWrite(ENA, LOW);

    pinMode(KICK, OUTPUT);
    digitalWrite(KICK, LOW);

    _rotationSpeed = 0;
}

void Controller::motor(int motor, int speed) {
    if (motor < 0 || motor > 3) {
        std::cerr << "Motor out of range!" << std::endl;
    }

    std::cout << speed << std::endl;

    _motors[motor].speed(speed);
}

void Controller::drive(const Vector2 &v) {
    _driveVector = v.clone();
}

void Controller::setRotation(double ω) {
    _rotationSpeed = ω;
}

template<typename D>
void Controller::kick(std::chrono::duration<D> duration) {
    using namespace std::chrono;

    if (duration.count() < 0 || duration.count() > MAX_KICK_STRENGTH) {
        std::cerr << "Strength out of range" << std::endl;
        return;
    }

    auto now = high_resolution_clock::now();

    if (auto elapsed = std::chrono::duration_cast<milliseconds>(now - _lastKick).count(); elapsed < 5000) {
        return;
    }

    _lastKick = now;

    digitalWrite(KICK, HIGH);
    std::this_thread::sleep_for(duration);
    digitalWrite(KICK, LOW);
}


void Controller::update() {
    updateDrive();
    updateIRData();
    updatePixyData();
}

Vector2 Controller::getBallVector() {
    return _ballVector.clone();
}

Vector2 Controller::getGoalVector() {
    return _goalVector.clone();
}

void Controller::updateDrive() {
    using enum MOTOR_CONFIGURATION;

    std::array<int, 3> motorSpeeds{};

    auto wSpeed = static_cast<int>(_rotationSpeed);

    double x = _driveVector.getX();
    double y = _driveVector.getY();

    double xAngle = sqrt(3) / 2;
    double yAngle = 0.5;

    motorSpeeds[std::to_underlying(RIGHT)] = static_cast<int>(xAngle * x + yAngle * y);
    motorSpeeds[std::to_underlying(LEFT)] = static_cast<int>(-xAngle * x + yAngle * y);
    motorSpeeds[std::to_underlying(BACK)] = static_cast<int>(-y);

    int maxSpeed = 0;

    for (const int motorSpeed: motorSpeeds) {
        maxSpeed = std::max(maxSpeed, std::abs(motorSpeed + wSpeed));
    }

    if (maxSpeed > 100) {
        for (int &motorSpeed: motorSpeeds) {
            motorSpeed = static_cast<int>(std::round(static_cast<double>(motorSpeed) / static_cast<double>(maxSpeed)));
        }
        wSpeed = static_cast<int>(std::round(static_cast<double>(wSpeed) / static_cast<double>(maxSpeed)));
    }

    for (size_t i = 0; i < motorSpeeds.size(); ++i) {
        _motors[i].speed(motorSpeeds[i] + wSpeed);
    }
}

void Controller::updateIRData() {
    Wire.requestFrom(IR_ADDRESS, 2);

    std::vector<int> data;

    while (Wire.available()) {
        data.push_back(Wire.read());
    }

    if (data.size() != 2) {
        std::cerr << "Data size mismatch!" << std::endl;
    }

    int dirRaw = data[0];
    int ballDist = data[1];

    if (dirRaw > 31) dirRaw -= 32;
    else dirRaw += 32;

    double ballDirDeg = dirRaw * 5.625;
    double ballDirRad = ballDirDeg * std::numbers::pi / 180;

    double ballX = std::cos(ballDirRad) * ballDist;
    double ballY = std::sin(ballDirRad) * ballDist;

    _ballVector.setX(ballX);
    _ballVector.setY(ballY);
}

void Controller::updatePixyData() {
    _pixy.ccc.getBlocks();

    if (_pixy.ccc.numBlocks) {
        auto const &b = _pixy.ccc.blocks[0];

        int dx = b.m_x - IMG_CENTER_X;
        int dy = IMG_CENTER_Y - b.m_y;

        _goalVector.setX(dy);
        _goalVector.setY(dx);

        return;
    }
    _goalVector.setX(-1);
    _goalVector.setY(-1);
}
