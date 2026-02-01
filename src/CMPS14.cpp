#include "CMPS14.hpp"

#include <Wire.h>
#include <iostream>

CMPS14::CMPS14(int address) {
    _address = address;

    _headingRad = 0.0;
    _headingDeg = 0.0;
    _originHeadingRad = 0.0;
}

double CMPS14::getHeadingRad() {
    return _headingRad;
}

double CMPS14::getHeadingDeg() {
    return _headingDeg;
}

void CMPS14::setOrigin() {
    update();
    _originHeadingRad = _headingRad;
}

void CMPS14::setOrigin(const double origin) {
    _originHeadingRad = origin;
}

void CMPS14::update() {
    uint16_t heading;

    Wire.beginTransmission(_address);
    Wire.write(HEADING_REGISTER);
    Wire.endTransmission(false);

    Wire.requestFrom(_address, 2);

    if (Wire.available() >= 2) {
        uint8_t high = Wire.read();
        uint8_t low = Wire.read();
        heading = (high << 8) | low;

        double headingDeg = heading / 10.0 - 180.0;
        double originDeg = _originHeadingRad / std::numbers::pi * 180.0;

        _headingDeg = wrapDeg(headingDeg - originDeg);
        _headingRad = _headingDeg * std::numbers::pi / 180.0;

        _headingRad = _headingDeg / 180.0 * std::numbers::pi;
    }
}

double CMPS14::wrapDeg(double deg) {
    deg = fmod(deg + 180.0, 360.0);
    if (deg < 0) deg += 360.0;
    return deg - 180.0;
}

