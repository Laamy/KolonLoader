#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include <filesystem>

#include "Console.h"

#ifdef KOLAN_EXPORTS
#define FILEIO_API __declspec(dllexport)
#else
#define FILEIO_API __declspec(dllimport)
#endif

class FILEIO_API FileIO {
private:
    std::string name;

public:
    // this is required if using FileIO for your clients files & assets
    bool setupClientPath(std::string clientName);
    bool doesClientPathExist(const std::string& path);
    void createPath(const std::string& path);
    void deletePath(const std::string& path);
    void writeFile(const std::string& filePath, const std::string& content);
    std::string getRoamingStatePath();
    std::string getClientPath();
    std::string readFile(const std::string& filePath, bool noError = false);
};

#endif // !FILEIO_H
