#pragma once

#include <deque>
#include <functional>

using delFunc = std::function<void()>;

class DeletionQueue {
public:
    void push_function(delFunc&& function);
    void push_function(delFunc& function);

    void flush();
private:
    std::deque<delFunc> deletors;
};