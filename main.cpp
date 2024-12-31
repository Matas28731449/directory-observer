#include <iostream>
#include <string>
#include "DirectoryObserver.h"

int main(const int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <directory-or-file-path>\n";
        return 1;
    }
    const std::string pathToMonitor = argv[1];

    DirectoryObserver observer;
    observer.addMonitor(pathToMonitor,
        [](const std::string& watchedPath, const std::vector<std::string>& changedEntities)
        {
            std::cout << "Detected changes in: " << watchedPath << std::endl;
            for (const auto& entity : changedEntities)
            {
                std::cout << "  " << entity << std::endl;
            }
            std::cout << std::endl;
        }
    );
    observer.start();

    std::cout << "Monitoring " << pathToMonitor << " for changes.\n";
    std::cout << "Press Enter to stop.\n";
    std::cin.get();

    observer.stop();
    return 0;
}