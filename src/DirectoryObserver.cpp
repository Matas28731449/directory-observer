#include "DirectoryObserver.h"

#include <iostream>
#include <chrono>
#include <thread>

DirectoryObserver::DirectoryObserver() : m_running(false) {}

DirectoryObserver::~DirectoryObserver() {
    stop();
}

void DirectoryObserver::addMonitor(const std::string& path, OnChangeCallback callback) {
    Target info;
    info.path = path;
    info.callback = callback;

    std::filesystem::path p(path);
    if (std::filesystem::exists(p)) {
        if (std::filesystem::is_directory(p)) {
            for (auto& entry : std::filesystem::directory_iterator(p)) {
                if (!std::filesystem::is_directory(entry.path())) {
                    info.entries[entry.path().filename().string()] = std::filesystem::last_write_time(entry.path());
                }
            }
        } else if (std::filesystem::is_regular_file(p)) {
            info.entries[p.filename().string()] = std::filesystem::last_write_time(p);
        }
    }
    m_targets.push_back(std::move(info));
}

void DirectoryObserver::start() {
    if (m_running) {
        return;
    }
    m_running = true;
    m_watchingThread = std::thread(&DirectoryObserver::monitorOnBackgroundThread, this);
}

void DirectoryObserver::stop() {
    m_running = false;
    if (m_watchingThread.joinable()) {
        m_watchingThread.join();
    }
}

void DirectoryObserver::monitorOnBackgroundThread() {
    while (m_running) {
        for (auto& target : m_targets) {
            std::filesystem::path p(target.path);
            if (!std::filesystem::exists(p)) {
                continue;
            }

            std::vector<std::string> changedFiles;
            std::unordered_map<std::string, std::filesystem::file_time_type> currentSnapshot;

            if (std::filesystem::is_directory(p)) {
                for (auto& entry : std::filesystem::directory_iterator(p)) {
                    if (std::filesystem::is_directory(entry.path())) {
                        continue;
                    }

                    auto filename = entry.path().filename().string();
                    auto lastWrite = std::filesystem::last_write_time(entry.path());
                    currentSnapshot[filename] = lastWrite;

                    if (target.entries.find(filename) == target.entries.end()) {
                        changedFiles.push_back(filename);
                    } else {
                        auto oldTime = target.entries[filename];
                        if (lastWrite != oldTime) {
                            changedFiles.push_back(filename);
                        }
                    }
                }
                for (auto& [filename, oldTime] : target.entries) {
                    if (currentSnapshot.find(filename) == currentSnapshot.end()) {
                        changedFiles.push_back(filename);
                    }
                }
            }
            else if (std::filesystem::is_regular_file(p)) {
                auto filename = p.filename().string();
                auto lastWrite = std::filesystem::last_write_time(p);
                currentSnapshot[filename] = lastWrite;

                if (target.entries.find(filename) == target.entries.end()) {
                    changedFiles.push_back(filename);
                } else if (target.entries[filename] != lastWrite) {
                    changedFiles.push_back(filename);
                }
            }

            target.entries = currentSnapshot;

            if (!changedFiles.empty()) {
                target.callback(target.path, changedFiles);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

