//
// Created by Raffael Bertram on 08.12.25.
//

#include "PID.hpp"

#include <Arduino.h>
#include <iostream>

PID::PID() {
    _targetPoint = 0;
    _kp = 0;
    _ki = 0;
    _kd = 0;
    _pTerm = 0;
    _iTerm = 0;
    _dTerm = 0;
    _lastError = 0;
    _lastValue = 0;
    _minOutput = 0;
    _maxOutput = 0;
    _intervalInMs = 1;
    _lastUpdateTime = std::chrono::steady_clock::now();
    _output = 0;
    _setpointChangedThreshold = 0;

    _state = PIDState::OFF;
    _direction = PIDDirection::DIRECT;
    _resetIOnSetpointChange = true;
    _conditionalITerm = false;
    _maxErrorITermUsed = 0;

    _mode = PIDMode::LINEAR;
    _targetMin = 0;
    _targetMax = 0;
}

void PID::setTargetPoint(double targetValue) {
    if (targetValue != _targetPoint) {
        if (_resetIOnSetpointChange && std::abs(targetValue - _targetPoint) > _setpointChangedThreshold) {
            _iTerm = 0;
        }

        _targetPoint = targetValue;
    }
}

void PID::setCoefficients(double kp, double ki, double kd) {
    _kp = kp >= 0 ? kp : -kp;
    _ki = ki >= 0 ? ki : -ki;
    _kd = kd >= 0 ? kd : -kd;

    _ki /= _intervalInMs;
    _kd *= _intervalInMs;

    if (_direction == PIDDirection::REVERSE) {
        _kp *= -1;
        _ki *= -1;
        _kd *= -1;
    }
}

void PID::setIntervalTime(int intervalTimeMs) {
    intervalTimeMs = intervalTimeMs > 0 ? intervalTimeMs : 1;

    double ratio = static_cast<double>(intervalTimeMs) / static_cast<double>(_intervalInMs);

    _ki /= ratio;
    _kd *= ratio;

    _intervalInMs = intervalTimeMs;
}

void PID::setOutputLimits(double min, double max) {
    if (min > max) {
        return;
    }

    _minOutput = min;
    _maxOutput = max;
}

void PID::setState(PIDState state) {
    if (state == _state) {
        return;
    }

    _state = state;

    if (_state == PIDState::OFF) {
        _iTerm = 0;
        return;
    }
    _firstExecution = true;
}

void PID::setDirection(PIDDirection direction) {
    if (_direction != direction) {
        _direction = direction;
        _kp *= -1;
        _ki *= -1;
        _kd *= -1;
    }
}

void PID::setMode(PIDMode mode, double targetMin, double targetMax) {
    _mode = mode;
    _targetMin = targetMin;
    _targetMax = targetMax;
}

void PID::setResetIOnSetpointChange(bool reset) {
    _resetIOnSetpointChange = reset;
}

void PID::setSetpointChangedThreshold(double threshold) {
    _setpointChangedThreshold = threshold;
}

void PID::setConditionalITerm(bool shouldBeConditional, double maxErrorITermUsed) {
    _conditionalITerm = shouldBeConditional;
    _maxErrorITermUsed = maxErrorITermUsed;
}

bool PID::isIntervalTimeElapsed() {
    int timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - _lastUpdateTime).count();

    return timeSinceLastUpdate > _intervalInMs;
}

void PID::calculateOutput(double error, double dError) {
    double currentICoefficient = _ki;

    if (_conditionalITerm && std::abs(error) > _maxErrorITermUsed) {
        currentICoefficient = 0;
    }

    _pTerm = _kp * error;
    _iTerm += currentICoefficient * error * _intervalInMs;
    _dTerm = _kd * dError / _intervalInMs;

    _output = _pTerm + _iTerm + _dTerm;
}

void PID::restrictOutput() {
    if (_output > _maxOutput) {
        _iTerm -= (_output - _maxOutput) * _ki * _intervalInMs;
        _output = _maxOutput;
    }
    if (_output < _minOutput) {
        _iTerm += (_minOutput - _output) * _ki * _intervalInMs;
        _output = _minOutput;
    }
}

void PID::restrictOutputLater(double outputWasLimitedTo) {
    if (_output > outputWasLimitedTo) {
        _iTerm -= (_output - outputWasLimitedTo) * _ki * _intervalInMs;
        _output = outputWasLimitedTo;
    }
    if (_output < outputWasLimitedTo) {
        _iTerm += (outputWasLimitedTo - _output) * _ki * _intervalInMs;
        _output = outputWasLimitedTo;
    }
}

double PID::calculateError(double currentValue) {
    double directError = currentValue - _targetPoint;

    if (_mode == PIDMode::LINEAR) {
        return directError;
    }

    double absWrappedDist = calculateAbsWrappedDiff(currentValue, _targetPoint);

    double wrappedError = currentValue < _targetPoint ? absWrappedDist : -absWrappedDist;

    return std::abs(wrappedError) > std::abs(directError) ? directError : wrappedError;
}

double PID::calculateDError(double currentValue) {
    double directDError = currentValue - _lastValue;

    if (_mode == PIDMode::LINEAR) {
        return directDError;
    }

    double absWrappedDist = calculateAbsWrappedDiff(currentValue, _lastValue);

    double wrappedDError = currentValue < _lastValue ? absWrappedDist : -absWrappedDist;

    return std::abs(wrappedDError) > std::abs(directDError) ? directDError : wrappedDError;
}

double PID::calculateAbsWrappedDiff(double value1, double value2) {
    double lowerValue = value1 < value2 ? value1 : value2;
    double upperValue = value1 < value2 ? value2 : value1;

    double diffUpper = _targetMax - upperValue;
    double diffLower = lowerValue - _targetMin;

    return diffLower + diffUpper;
}

void PID::compute(double currentValue) {
    if (!isIntervalTimeElapsed()) {
        return;
    }
    _lastUpdateTime = std::chrono::steady_clock::now();

    if (_state == PIDState::OFF) {
        return;
    }

    double error = calculateError(currentValue);

    if (_firstExecution) {
        _firstExecution = false;
        _lastValue = currentValue;
        _lastError = error;
    }

    if (std::copysign(1, error) != std::copysign(1, _lastError)) {
        _iTerm = 0;
    }

    double dError = calculateDError(currentValue);

    calculateOutput(error, dError);

    restrictOutput();

    _lastError = error;
    _lastValue = currentValue;
}

double PID::getOutput() {
    return _output;
}
