#include "SchedulerModule.h"

#include <chrono>
#include <utility>

namespace scheduler {

    SchedulerModule::SchedulerModule(
        std::string name,
        ModuleConfig config
    )
        : name(std::move(name)),
          config(config),
          enabled(config.startEnabled) {}

    SchedulerModule::~SchedulerModule() {
        StopDedicated();
    }

    const std::string& SchedulerModule::GetName() const {
        return name;
    }

    const ModuleConfig& SchedulerModule::GetConfig() const {
        return config;
    }

    bool SchedulerModule::IsEnabled() const {
        return enabled.load();
    }

    void SchedulerModule::SetEnabled(bool value) {
        enabled.store(value);
        queue.NotifyAll();
    }

    TaskQueue& SchedulerModule::Queue() {
        return queue;
    }

    void SchedulerModule::SetExecutor(
        std::function<void(ScheduledTask)> value
    ) {
        executor = std::move(value);
    }

    void SchedulerModule::SetErrorHandler(
        std::function<void(const std::string&)> value
    ) {
        errorHandler = std::move(value);
    }

    void SchedulerModule::ReportError(
        const std::string& message
    ) const {
        if (errorHandler) {
            errorHandler(name + ": " + message);
        }
    }

    void SchedulerModule::StartDedicated() {
        if (config.executionPolicy != ExecutionPolicy::DedicatedThread ||
            thread.joinable()) {
            return;
        }

        stopping.store(false);
        thread = std::thread(&SchedulerModule::DedicatedLoop, this);
    }

    void SchedulerModule::StopDedicated() {
        stopping.store(true);
        queue.NotifyAll();

        if (thread.joinable() &&
            thread.get_id() != std::this_thread::get_id()) {
            thread.join();
        }
    }

    void SchedulerModule::DedicatedLoop() {
        while (!stopping.load()) {
            if (!enabled.load()) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(2)
                );
                continue;
            }

            ScheduledTask task;

            if (!queue.WaitPopReady(task, stopping)) {
                break;
            }

            if (task.cancelled->load()) {
                continue;
            }

            if (executor) {
                executor(std::move(task));
            }
        }
    }

} // namespace scheduler