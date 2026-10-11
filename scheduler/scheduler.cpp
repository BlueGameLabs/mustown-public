#include "Scheduler.h"

#include <algorithm>
#include <chrono>
#include <exception>
#include <thread>
#include <utility>

namespace scheduler {

    Scheduler::Scheduler(SchedulerConfig config)
        : config(config) {
        if (this->config.workerCount == 0) {
            this->config.workerCount =
                std::thread::hardware_concurrency();

            if (this->config.workerCount == 0) {
                this->config.workerCount = 1;
            }
        }
    }

    Scheduler::~Scheduler() {
        Stop();
    }

    bool Scheduler::ConfigureModule(
        const std::string& name,
        ModuleConfig config
    ) {
        if (name.empty() || running.load()) {
            return false;
        }

        std::lock_guard<std::mutex> lock(mutex);

        if (modules.find(name) != modules.end()) {
            return false;
        }

        auto module = std::make_unique<SchedulerModule>(
            name,
            config
        );

        module->SetExecutor(
            [this, name](ScheduledTask task) {
                SchedulerModule* target = nullptr;

                {
                    std::lock_guard<std::mutex> lock(mutex);
                    auto it = modules.find(name);

                    if (it != modules.end()) {
                        target = it->second.get();
                    }
                }

                if (target) {
                    ExecuteTask(*target, std::move(task));
                }
            }
        );

        module->SetErrorHandler([this](const std::string&) {
            failed.fetch_add(1);
        });

        modules.emplace(name, std::move(module));
        return true;
    }

    bool Scheduler::Start() {
        bool expected = false;

        if (!running.compare_exchange_strong(expected, true)) {
            return false;
        }

        stopping.store(false);

        try {
            std::lock_guard<std::mutex> lock(mutex);

            for (auto& pair : modules) {
                if (pair.second->IsEnabled() &&
                    pair.second->GetConfig().executionPolicy ==
                        ExecutionPolicy::DedicatedThread) {
                    pair.second->StartDedicated();
                }
            }

            for (std::size_t i = 0; i < config.workerCount; ++i) {
                workers.emplace_back(&Scheduler::WorkerLoop, this);
            }
        } catch (...) {
            Stop();
            return false;
        }

        return true;
    }

    void Scheduler::Stop() {
        if (!running.exchange(false)) {
            return;
        }

        stopping.store(true);

        {
            std::lock_guard<std::mutex> lock(mutex);

            for (auto& pair : modules) {
                pair.second->StopDedicated();
                pair.second->Queue().NotifyAll();
            }
        }

        for (auto& worker : workers) {
            if (worker.joinable() &&
                worker.get_id() != std::this_thread::get_id()) {
                worker.join();
            }
        }

        workers.clear();

        {
            std::lock_guard<std::mutex> lock(mutex);

            for (auto& pair : modules) {
                pair.second->Queue().Clear();
            }

            cancellationTokens.clear();
        }

        stopping.store(false);
    }

    bool Scheduler::IsRunning() const {
        return running.load();
    }

    TaskId Scheduler::Schedule(
        const std::string& moduleName,
        std::function<void()> callback,
        TaskOptions options
    ) {
        if (!running.load() || !callback ||
            options.delay.count() < 0 ||
            options.interval.count() < 0 ||
            (options.repeating && options.interval.count() <= 0)) {
            return 0;
        }

        std::lock_guard<std::mutex> lock(mutex);

        auto it = modules.find(moduleName);

        if (it == modules.end() || !it->second->IsEnabled()) {
            return 0;
        }

        std::size_t queued = 0;

        for (const auto& pair : modules) {
            queued += pair.second->Queue().Size();
        }

        if (config.maxQueuedTasks > 0 &&
            queued >= config.maxQueuedTasks) {
            return 0;
        }

        ScheduledTask task;
        task.id = nextTaskId.fetch_add(1);
        task.callback = std::move(callback);
        task.options = options;
        task.nextRun = Clock::now() + options.delay;

        const TaskId id = task.id;
        cancellationTokens[id] = task.cancelled;

        if (!it->second->Queue().Push(
                std::move(task),
                it->second->GetConfig().queueCapacity)) {
            cancellationTokens.erase(id);
            return 0;
        }

        return id;
    }

    bool Scheduler::Cancel(TaskId id) {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = cancellationTokens.find(id);

        if (it == cancellationTokens.end()) {
            return false;
        }

        if (auto token = it->second.lock()) {
            token->store(true);
        }

        cancellationTokens.erase(it);
        return true;
    }

    bool Scheduler::SetModuleEnabled(
        const std::string& name,
        bool enabled
    ) {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = modules.find(name);

        if (it == modules.end()) {
            return false;
        }

        auto& module = *it->second;
        module.SetEnabled(enabled);

        if (running.load() &&
            module.GetConfig().executionPolicy ==
                ExecutionPolicy::DedicatedThread) {
            if (enabled) {
                module.StartDedicated();
            } else {
                module.StopDedicated();
            }
        }

        return true;
    }

    SchedulerMetrics Scheduler::GetMetrics() const {
        SchedulerMetrics result;

        {
            std::lock_guard<std::mutex> lock(mutex);

            for (const auto& pair : modules) {
                result.queuedTasks += pair.second->Queue().Size();
            }
        }

        result.completedTasks = completed.load();
        result.failedTasks = failed.load();

        return result;
    }

    void Scheduler::WorkerLoop() {
        while (!stopping.load()) {
            SchedulerModule* selected = nullptr;
            ScheduledTask task;
            bool found = false;

            {
                std::lock_guard<std::mutex> lock(mutex);

                for (auto& pair : modules) {
                    auto& module = *pair.second;

                    if (!module.IsEnabled() ||
                        module.GetConfig().executionPolicy !=
                            ExecutionPolicy::ThreadPool) {
                        continue;
                    }

                    if (module.Queue().TryPopReady(
                            Clock::now(), task)) {
                        selected = &module;
                        found = true;
                        break;
                    }
                }
            }

            if (found && selected) {
                if (!task.cancelled->load()) {
                    ExecuteTask(*selected, std::move(task));
                }
            } else {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(1)
                );
            }
        }
    }

    void Scheduler::ExecuteTask(
        SchedulerModule& module,
        ScheduledTask task
    ) {
        if (task.cancelled->load() || stopping.load()) {
            return;
        }

        try {
            task.callback();
            completed.fetch_add(1);
        } catch (const std::exception& e) {
            failed.fetch_add(1);
            module.ReportError(e.what());
        } catch (...) {
            failed.fetch_add(1);
            module.ReportError("Unknown task exception");
        }

        if (task.options.repeating &&
            !task.cancelled->load() &&
            !stopping.load() &&
            module.IsEnabled()) {
            task.nextRun = Clock::now() + task.options.interval;
            module.Queue().Push(
                std::move(task),
                module.GetConfig().queueCapacity
            );
        } else {
            std::lock_guard<std::mutex> lock(mutex);
            cancellationTokens.erase(task.id);
        }
    }

} // namespace scheduler