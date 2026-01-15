#include <Arduino.h>
#include <Wire.h>
#include <iostream>

#include "CMPS14.hpp"

CMPS14 cmps14(0x60);

void setup() {
    Wire.begin();
    Serial.begin(115200);

    cmps14.setOrigin();
}

void loop() {
    cmps14.update();

    std::cout << cmps14.getHeadingDeg() << std::endl;

    delay(10);
}
