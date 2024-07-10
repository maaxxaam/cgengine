#pragma once

#include <optional>
#include <vector>

#include "error.h"

struct Timer {
    float time;
    std::function<std::optional<Error *> ()> onTimer;
};

class TimerStorage {
public:
    int queuedTimers() const { return _timers.size(); };
    void addTimer(Timer& timer);
    void addTimer(float time, std::function<std::optional<Error *> ()>& onTimer);
    void addTimer(float time, std::function<std::optional<Error *> ()>&& onTimer);
    MaybeMultipleErrors update(float delta);
private:
    std::vector<Timer> _timers;
};
