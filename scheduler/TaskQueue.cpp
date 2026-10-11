#include "TaskQueue.h"

namespace scheduler {

    bool TaskQueue::Push(
        ScheduledTask task,
        std::size_t capacity
    ) {
        {
            std::lock_guard<std::mutex> lock(mutex);

            if (capacity > 0 && tasks.size() >= capacity) {
                return false;
            }

            tasks.push(std::move(task));
        }

        cv.notify_all();
        return true;
    }

    bool TaskQueue::TryPopReady(
        Clock::time_point now,
        ScheduledTask& out
    ) {
        std::lock_guard<std::mutex> lock(mutex);

        if (tasks.empty() || tasks.top().nextRun > now) {
            return false;
        }

        out = tasks.top();
        tasks.pop();
        return true;
    }

    bool TaskQueue::WaitPopReady(
        ScheduledTask& out,
        const std::atomic<bool>& stopping
    ) {
        std::unique_lock<std::mutex> lock(mutex);

        while (!stopping.load()) {
            if (tasks.empty()) {
                cv.wait(lock, [&] {
                    return stopping.load() || !tasks.empty();
                });

                continue;
            }

            const auto due = tasks.top().nextRun;

            if (Clock::now() >= due) {
                out = tasks.top();
                tasks.pop();
                return true;
            }

            cv.wait_until(lock, due);
        }

        return false;
    }

    void TaskQueue::NotifyAll() {
        cv.notify_all();
    }

    void TaskQueue::Clear() {
        std::lock_guard<std::mutex> lock(mutex);

        decltype(tasks) empty;
        tasks.swap(empty);

        cv.notify_all();
    }

    std::size_t TaskQueue::Size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return tasks.size();
    }

} // namespace scheduler