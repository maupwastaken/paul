#include <iostream>

#include "WorldInfo.hpp"

std::unique_ptr<WorldInfo> worldInfo;

void setup() {
    worldInfo = std::make_unique<WorldInfo>(false);
}

void loop() {
    worldInfo->update();

    std::cout << worldInfo->getBallVector()
            << " " << worldInfo->isBallAligned()
            << " " << worldInfo->botBehindBall()
            << " " << worldInfo->getHasBall()
            << " " << worldInfo->getCurrentHeading()
            << std::endl;
}
