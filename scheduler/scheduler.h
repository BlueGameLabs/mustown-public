#ifndef MUSTOWN_SCHEDULER_H
#define MUSTOWN_SCHEDULER_H

#include "SchedulerConfig.h"
#include "SchedulerModule.h"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace scheduler {

    struct SchedulerMetrics {
        std::size_t queuedTasks = 0;
        std::uint64_t completedTasks = 0;
        std::uint64_t failedTasks = 0;
    };

    class Scheduler {
    public:
        explicit Scheduler(SchedulerConfig config = {});
        ~Scheduler();

        Scheduler(const Scheduler&) = delete;
        Scheduler& operator=(const Scheduler&) = delete;

        bool ConfigureModule(
            const std::string& name,
            ModuleConfig config
        );

        bool Start();
        void Stop();

        bool IsRunning() const;

        TaskId Schedule(
            const std::string& module,
            std::function<void()> callback,
            TaskOptions options = {}
        );

        bool Cancel(TaskId id);

        bool SetModuleEnabled(
            const std::string& module,
            bool enabled
        );

        SchedulerMetrics GetMetrics() const;

    private:
        void WorkerLoop();
        void ExecuteTask(
            SchedulerModule& module,
            ScheduledTask task
        );

        SchedulerConfig config;

        mutable std::mutex mutex;

        std::unordered_map<
            std::string,
            std::unique_ptr<SchedulerModule>
        > modules;

        std::unordered_map<
            TaskId,
            std::weak_ptr<std::atomic<bool>>
        > cancellationTokens;

        std::vector<std::thread> workers;

        std::atomic<bool> running{false};
        std::atomic<bool> stopping{false};
        std::atomic<TaskId> nextTaskId{1};

        std::atomic<std::uint64_t> completed{0};
        std::atomic<std::uint64_t> failed{0};
    };

} // namespace scheduler

#endif // MUSTOWN_SCHEDULER_H