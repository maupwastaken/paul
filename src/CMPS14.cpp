#include "CMPS14.hpp"

#include <Wire.h>
#include <iostream>

CMPS14::CMPS14(int address) {
    _address = address;

    _headingRad = 0.0;
    _headingDeg = 0.0;
    _rawHeadingDeg = 0.0;
    _originHeadingDeg = 0.0;
}

double CMPS14::getHeadingRad() {
    return _headingRad;
}

double CMPS14::getHeadingDeg() {
    return _headingDeg;
}

void CMPS14::setOrigin() {
    update();                 // get fresh raw value
    _originHeadingDeg = _rawHeadingDeg;
}

void CMPS14::setOrigin(const double origin) {
    _originHeadingDeg = origin / std::numbers::pi * 180;
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

        _rawHeadingDeg = heading / 10.0 - 180.0;

        _headingDeg = wrapDeg(_rawHeadingDeg - _originHeadingDeg);
        _headingRad = _headingDeg * M_PI / 180.0;
    }
}


double CMPS14::wrapDeg(double deg) {
    deg = fmod(deg + 180.0, 360.0);
    if (deg < 0) deg += 360.0;
    return deg - 180.0;
}

