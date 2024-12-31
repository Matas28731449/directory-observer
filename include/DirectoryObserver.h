#pragma once

#include <string>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <filesystem>

class DirectoryObserver {
public:
    using OnChangeCallback = std::function<void(const std::string&, const std::vector<std::string>&)>;

    DirectoryObserver();
    ~DirectoryObserver();

    void addMonitor(const std::string& path, OnChangeCallback callback);

    void start();

    void stop();

private:

    struct Target {
        std::string path;
        OnChangeCallback callback;
        std::unordered_map<std::string, std::filesystem::file_time_type> entries;
    };

    void monitorOnBackgroundThread();

    std::vector<Target> m_targets;

    std::thread m_watchingThread;
    std::atomic<bool> m_running;
};