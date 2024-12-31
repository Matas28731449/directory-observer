#include <iostream>
#include "DirectoryObserver.h"

int main() {
    DirectoryObserver observer;

    observer.addMonitor(
        "/Users/matas.lekavicius/monitoring_test_directory/random.txt",
        [](const std::string& path, const std::vector<std::string>& changedFiles) {
            std::cout << "Changes in [" << path << "]:\n";
            for (auto& file : changedFiles) {
                std::cout << " - " << file << "\n";
            }
            std::cout << std::endl;
        }
    );

    observer.start();

    std::cout << "Monitoring /Users/matas.lekavicius/monitoring_test_directory/random.txt for changes...\n";
    std::cout << "Press any key to stop monitoring.\n";
    std::cin.get();

    observer.stop();

    return 0;
}