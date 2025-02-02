#pragma once

#include <functional>

#include "eastloverloads.h"
#include <EASTL/deque.h>

using delFunc = std::function<void()>;

class DeletionQueue {
public:
    void push_function(delFunc&& function);
    void push_function(delFunc& function);

    void flush();
private:
    eastl::deque<delFunc> deletors;
};