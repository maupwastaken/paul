//
// Created by Jan Schlegel on 01.02.26.
//

#pragma once

#include <chrono>
#include <memory>

#include "Vector2.hpp"
#include "CMPS14.hpp"
#include "Pixy2I2C.h"

class WorldInfo {
public:
    WorldInfo(bool useLightBarrier);

    void update();

    void setHeadingOrigin() const;

    void setHeadingOrigin(double newHeading) const;

    [[nodiscard]] double getCurrentHeading() const;

    [[nodiscard]] Vector2 getBallVector() const;

    [[nodiscard]] Vector2 getGoalVector() const;

    [[nodiscard]] bool isBallAligned() const;

    [[nodiscard]] bool botBehindBall() const;

    [[nodiscard]] bool getHasBall() const;

    [[nodiscard]] bool getGoalExists() const;

private:
    void updateCMPS14Data();

    void updateIRData();

    void updatePixyData();

    void updateBallData();

    static bool isPixyRunning();

    std::unique_ptr<CMPS14> _compass;

    std::unique_ptr<Pixy2I2C> _pixy;

    double _resX;
    double _resY;

    double _currentHeading;

    Vector2 _ballVector;
    Vector2 _goalVector;

    bool _isBallAligned;
    bool _botBehindBall;
    bool _hasBall;

    bool _goalExists;

    bool _usingLightBarrier;

    std::chrono::time_point<std::chrono::high_resolution_clock> _ballLastNotAlignedTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> _goalLastSeenTime;

    bool _isPixyRunning;
};
