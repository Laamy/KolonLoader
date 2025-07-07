#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>

class __declspec(dllexport) FileIO {
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