//
// Created by Jan Schlegel on 01.02.26.
//

#include "WorldInfo.hpp"
#include "Settings.hpp"

#include <iostream>

WorldInfo::WorldInfo(bool useLightBarrier) {
    Wire.begin();

    delay(1000);

    _compass = std::make_unique<CMPS14>(CMPS14_ADDRESS);
    _compass->setOrigin();
    _isPixyRunning = isPixyRunning();

    if (_isPixyRunning) {
        _pixy = std::make_unique<Pixy2I2C>();

        if (_pixy->init(PIXY_ADDRESS) != PIXY_RESULT_OK) {
            _isPixyRunning = false;
        }

        _pixy->getResolution();

        _resX = _pixy->frameWidth;
        _resX = _pixy->frameHeight;
    } else {
        _pixy = nullptr;

        _resX = 0.0;
        _resY = 0.0;
    }

    _currentHeading = 0.0;

    _ballVector = Vector2(0, 0);
    _goalVector = Vector2(0, 0);

    _isBallAligned = false;
    _botBehindBall = false;
    _hasBall = false;

    _goalExists = false;

    _usingLightBarrier = useLightBarrier;

    if (_usingLightBarrier) {
        pinMode(LIGHT_BARRIER_PIN, INPUT);
    }
}

void WorldInfo::update() {
    updateCMPS14Data();
    updateIRData();
    updateBallData();

    if (_isPixyRunning) {
        updatePixyData();
    }
}

void WorldInfo::setHeadingOrigin() const {
    _compass->setOrigin();
}

void WorldInfo::setHeadingOrigin(const double newHeading) const {
    _compass->setOrigin(newHeading);
}

double WorldInfo::getCurrentHeading() const {
    return _currentHeading;
}

Vector2 WorldInfo::getBallVector() const {
    return _ballVector;
}

Vector2 WorldInfo::getGoalVector() const {
    return _goalVector;
}

bool WorldInfo::isBallAligned() const {
    return _isBallAligned;
}

bool WorldInfo::botBehindBall() const {
    return _botBehindBall;
}

bool WorldInfo::getHasBall() const {
    return _hasBall;
}

bool WorldInfo::getGoalExists() const {
    return _goalExists;
}

void WorldInfo::updateCMPS14Data() {
    _compass->update();

    _currentHeading = _compass->getHeadingRad();
}

void WorldInfo::updateIRData() {
    Wire.requestFrom(IR_ADDRESS, sizeof(float) * 2);

    std::vector<uint8_t> data;

    while (Wire.available()) {
        data.push_back(static_cast<uint8_t>(Wire.read()));
    }

    if (data.size() != sizeof(float) * 2) {
        std::cerr << "Data size mismatch!" << std::endl;
        return;
    }

    float dir = 0.0f;
    float dist = 0.0f;

    memcpy(&dir, data.data(), sizeof(float));
    memcpy(&dist, data.data() + sizeof(float), sizeof(float));

    double ballX = std::cos(dir) * dist;
    double ballY = std::sin(dir) * dist;

    _ballVector.setX(ballX);
    _ballVector.setY(ballY);
}

void WorldInfo::updatePixyData() {
    _pixy->ccc.getBlocks(false);

    if (_pixy->ccc.numBlocks) {
        const auto b = _pixy->ccc.blocks[0];

        const int dx = b.m_x - 316 / 2;
        const int dy = 208 / 2 - b.m_y;

        _goalVector.setX(dy);
        _goalVector.setY(dx);



        _goalExists = true;

        return;
    }

    _goalExists = false;
}

void WorldInfo::updateBallData() {
    const auto now = std::chrono::high_resolution_clock::now();

    _isBallAligned = std::abs(_ballVector.getAngle()) < std::numbers::pi / 8.0;
    _botBehindBall = _ballVector.getSignX() == 1;

    if (_usingLightBarrier) {
        _hasBall = analogRead(LIGHT_BARRIER_PIN) > LIGHT_BARRIER_THRESHOLD;
        return;
    }

    if (!_isBallAligned) {
        _ballLastNotAlignedTime = now;
        _hasBall = false;
        return;
    }

    _hasBall = now - _ballLastNotAlignedTime > BALL_ALIGNED_THRESHOLD;
}

bool WorldInfo::isPixyRunning() {
    Wire.beginTransmission(PIXY_ADDRESS);
    return Wire.endTransmission() == 0;
}
