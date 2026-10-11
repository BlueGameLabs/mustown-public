#ifndef MUSTOWN_SCHEDULEDTASK_H
#define MUSTOWN_SCHEDULEDTASK_H

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

namespace scheduler {

    using TaskId = std::uint64_t;
    using Clock = std::chrono::steady_clock;

    struct TaskOptions {
        std::chrono::milliseconds delay{0};
        std::chrono::milliseconds interval{0};
        bool repeating = false;
        int priority = 0;
    };

    struct ScheduledTask {
        TaskId id = 0;
        std::function<void()> callback;
        TaskOptions options;
        Clock::time_point nextRun = Clock::now();

        std::shared_ptr<std::atomic<bool>> cancelled =
            std::make_shared<std::atomic<bool>>(false);
    };

} // namespace scheduler

#endif // MUSTOWN_SCHEDULEDTASK_H