#include "timer.h"
#include <algorithm>

MaybeMultipleErrors TimerStorage::update(float delta) {
    std::vector<Error *> errors{};
    for (auto item = _timers.rbegin(); item != _timers.rend(); item++) {
        if (item->time < delta) {
            item->time = 0;
            std::optional<Error *> error = item->onTimer();
            if (error) {
                errors.push_back(error.value());
            }
        } else {
            item->time -= delta;
        }
    }
    _timers.erase(
    std::remove_if(_timers.begin(), _timers.end(),
        [](const Timer & o) { return o.time <= 0; }),
    _timers.end());
    if (errors.empty())
        return {};
    else 
        return errors;
}

void TimerStorage::addTimer(Timer& timer) {
    _timers.push_back(timer);
}

void TimerStorage::addTimer(float time, std::function<std::optional<Error *> ()>& onTimer) {
    _timers.push_back({ time, onTimer});
}

void TimerStorage::addTimer(float time, std::function<std::optional<Error *> ()>&& onTimer) {
    _timers.push_back({ time, onTimer});
}
