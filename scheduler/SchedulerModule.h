#ifndef MUSTOWN_SCHEDULERMODULE_H
#define MUSTOWN_SCHEDULERMODULE_H

#include "SchedulerConfig.h"
#include "TaskQueue.h"

#include <atomic>
#include <functional>
#include <string>
#include <thread>

namespace scheduler {

    class SchedulerModule {
    public:
        SchedulerModule(
            std::string name,
            ModuleConfig config
        );

        ~SchedulerModule();

        SchedulerModule(const SchedulerModule&) = delete;
        SchedulerModule& operator=(const SchedulerModule&) = delete;

        const std::string& GetName() const;
        const ModuleConfig& GetConfig() const;

        bool IsEnabled() const;
        void SetEnabled(bool enabled);

        TaskQueue& Queue();

        void StartDedicated();
        void StopDedicated();

        void SetExecutor(
            std::function<void(ScheduledTask)> executor
        );

        void SetErrorHandler(
            std::function<void(const std::string&)> handler
        );

        void ReportError(const std::string& message) const;

    private:
        void DedicatedLoop();

        std::string name;
        ModuleConfig config;

        std::atomic<bool> enabled;
        std::atomic<bool> stopping{false};

        std::thread thread;
        TaskQueue queue;

        std::function<void(ScheduledTask)> executor;
        std::function<void(const std::string&)> errorHandler;
    };

} // namespace scheduler

#endif // MUSTOWN_SCHEDULERMODULE_H