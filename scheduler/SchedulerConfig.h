#ifndef MUSTOWN_SCHEDULERCONFIG_H
#define MUSTOWN_SCHEDULERCONFIG_H

#include <cstddef>

namespace scheduler {

    enum class ExecutionPolicy {
        ThreadPool,
        DedicatedThread
    };

    struct ModuleConfig {
        ExecutionPolicy executionPolicy = ExecutionPolicy::ThreadPool;
        std::size_t queueCapacity = 0; // 0 means unlimited.
        bool startEnabled = true;
    };

    struct SchedulerConfig {
        std::size_t workerCount = 0; // 0 uses hardware concurrency.
        std::size_t maxQueuedTasks = 0; // 0 means unlimited.
    };

} // namespace scheduler

#endif // MUSTOWN_SCHEDULERCONFIG_H