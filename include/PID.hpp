//
// Created by Raffael Bertram on 08.12.25.
//

#pragma once

#include <chrono>
#include <cmath>

enum class PIDState : bool {
    OFF = 0,
    ON = 1
};

enum class PIDDirection : bool {
    DIRECT = 0,
    REVERSE = 1
};

enum class PIDMode : bool {
    LINEAR = 0,
    CIRCULAR = 1
};

class PID {
public:
    PID();

    void setTargetPoint(double targetValue);

    void setState(PIDState state);

    void setDirection(PIDDirection direction);

    void setCoefficients(double kp, double ki, double kd);

    void setIntervalTime(int intervalTimeMs);

    void setMode(PIDMode mode, double targetMin, double targetMax);

    void setOutputLimits(double min, double max);

    void restrictOutputLater(double outputWasLimitedTo);

    void setResetIOnSetpointChange(bool reset);

    void setSetpointChangedThreshold(double threshold);

    void setConditionalITerm(bool shouldBeConditional, double maxErrorITermUsed);

    void compute(double currentValue);

    double getOutput();

private:
    bool isIntervalTimeElapsed();

    double calculateError(double currentValue);

    double calculateDError(double currentValue);

    double calculateAbsWrappedDiff(double value1, double value2);

    void calculateOutput(double error, double dError);

    void restrictOutput();

    double _targetPoint;
    double _kp;
    double _ki;
    double _kd;
    double _pTerm;
    double _iTerm;
    double _dTerm;
    double _lastError;
    double _lastValue;
    double _minOutput;
    double _maxOutput;
    double _setpointChangedThreshold;
    int _intervalInMs;
    std::chrono::time_point<std::chrono::steady_clock> _lastUpdateTime;
    double _output;

    bool _firstExecution;
    PIDState _state;
    PIDDirection _direction;
    bool _resetIOnSetpointChange;
    bool _conditionalITerm;
    double _maxErrorITermUsed;
    PIDMode _mode;

    double _targetMin;
    double _targetMax;
};