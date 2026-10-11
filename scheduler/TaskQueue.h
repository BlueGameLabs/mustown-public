#ifndef MUSTOWN_TASKQUEUE_H
#define MUSTOWN_TASKQUEUE_H

#include "ScheduledTask.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

namespace scheduler {

    class TaskQueue {
    public:
        bool Push(ScheduledTask task, std::size_t capacity = 0);

        bool TryPopReady(
            Clock::time_point now,
            ScheduledTask& out
        );

        bool WaitPopReady(
            ScheduledTask& out,
            const std::atomic<bool>& stopping
        );

        void NotifyAll();
        void Clear();
        std::size_t Size() const;

    private:
        struct Compare {
            bool operator()(
                const ScheduledTask& a,
                const ScheduledTask& b
            ) const {
                if (a.nextRun != b.nextRun) {
                    return a.nextRun > b.nextRun;
                }

                return a.options.priority < b.options.priority;
            }
        };

        mutable std::mutex mutex;
        std::condition_variable cv;

        std::priority_queue<
            ScheduledTask,
            std::vector<ScheduledTask>,
            Compare
        > tasks;
    };

} // namespace scheduler

#endif // MUSTOWN_TASKQUEUE_H