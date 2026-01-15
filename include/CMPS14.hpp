//
// Created by Jan Schlegel on 15.01.26.
//

#pragma once

#include <utility>
#include <cstdint>

inline constexpr int HEADING_REGISTER = 0x02;

class CMPS14 {
public:
    explicit CMPS14(int address);

    double getHeadingRad();

    double getHeadingDeg();

    void setOrigin();

    void update();

private:
    uint8_t _address;

    double _headingRad;
    double _headingDeg;

    double _originHeadingRad;
};
