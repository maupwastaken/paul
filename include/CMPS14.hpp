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

    void setOrigin(double origin);

    void update();

private:
    static double wrapDeg(double deg);

    uint8_t _address;

    double _headingRad;
    double _headingDeg;

    double _originHeadingRad;
};
