#include "FileIO.h"

#include "Console.h"

#include <iostream>
#include <fstream>
#include <filesystem>

// this is required if using FileIO for your clients files & assets
inline bool FileIO::setupClientPath(std::string clientName)
{
    if (clientName.empty()) {
        Console::Log("KalonLoader.Core", "Client name is empty, please provide a valid name.");
        return false;
    }
    name = clientName;

    if (!std::filesystem::exists(getClientPath())) {
        std::filesystem::create_directory(getClientPath());
        return true;
    }
    return false;
}

inline std::string FileIO::getRoamingStatePath()
{
    // might add smth for preview clients
    return (getenv("AppData") + (std::string)"\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState\\");
}

inline std::string FileIO::getClientPath()
{
    if (name.empty())
    {
        Console::Log("KalonLoader.Core", "Client name is not set, please call setupClientPath() first.");
        return "";
    }

    return getRoamingStatePath() + name + "\\";
}

inline bool FileIO::doesClientPathExist(const std::string& path)
{
    return std::filesystem::exists(getClientPath() + path);
}

inline void FileIO::createPath(const std::string& path)
{
    std::filesystem::create_directory(getClientPath() + path);
}

inline void FileIO::deletePath(const std::string& path)
{
    std::filesystem::remove(path);
}

inline void FileIO::writeFile(const std::string& filePath, const std::string& content)
{
    std::ofstream file(getClientPath() + filePath);
    if (file.is_open()) {
        file << content;
        file.close();
    }
    else Console::Log("KalonLoader.Core", "Unable to open the file for writing: %s", filePath.c_str());
}

inline std::string FileIO::readFile(const std::string& filePath, bool noError)
{
    std::ifstream file(getClientPath() + filePath);
    std::string content;
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (content.size() > 1) {
                content += "\r\n";
            }

            content += line;
        }
        file.close();
    }
    else {
        if (noError) {
            return content;
        }

        Console::Log("KolanLoader.Core", "Unable to open the file for reading");
    }
    return content;
}
