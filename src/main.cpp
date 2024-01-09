#include <iostream>
#include <cstdint>
#include <dirent.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "../include/process.h"

// This is used to get the total CPU time for all processes
uint64_t getTotalCpuTime() {
    std::ifstream statFile("/proc/stat");

    if (statFile.is_open()) {
        std::string line;
        std::getline(statFile, line);
        statFile.close();

        std::istringstream iss(line);

        std::string cpu;
        uint64_t user, nice, system, idle, iowait, irq, softirq, steal;

        iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

        return user + nice + system + idle + iowait + irq + softirq + steal;
    }
    else {
        std::cerr << "Error: Could not open /proc/stat" << std::endl;
        return 0;
    }
}

// This is used to get the CPU time for a specific process
uint64_t getProcessCpuTime(const uint32_t& pid) {
    std::ifstream statFile("/proc/" + std::to_string(pid) + "/stat");

    if (statFile.is_open()) {
        std::string line;
        std::getline(statFile, line);
        statFile.close();

        std::istringstream iss(line);

        for (uint32_t i = 0; i < 13; ++i) {
            std::string temp;
            iss >> temp;
        }

        uint64_t utime, stime;
        iss >> utime >> stime;

        return (utime + stime);
    }
    else {
        std::cerr << "Error: Could not open /proc/" << pid << "/stat" << std::endl;
        return 0;
    }
}

// This is used to get the memory usage for a specific process
uint64_t getProcessMem(const uint32_t& pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/statm";

    std::ifstream statmFile(path);

    if (statmFile.is_open()) {
        std::string line;
        std::getline(statmFile, line);
        statmFile.close();

        std::istringstream iss(line);

        uint64_t mem;
        iss >> mem;

        return mem;
    }
    else {
        std::cerr << "Error: Could not open " << path << std::endl;
        return 0;
    }
}

// This is used to get the process information
Process getProcessInfo(const uint32_t& pid, const uint64_t& totalCpuTime) {
    Process process;
    uint64_t processCpuTime = getProcessCpuTime(pid);
    uint32_t cpu = static_cast<uint32_t>((processCpuTime * 100) / (totalCpuTime));
    uint32_t mem = static_cast<uint32_t>(getProcessMem(pid));

    return Process(pid, cpu, mem);
}

// This is used to get all the processes with their information
std::vector<Process> getProccesses() {
    std::vector<Process> processes;

    DIR* dir = opendir("/proc");
    if (dir == nullptr) {
        std::cerr << "Error: Could not open /proc" << std::endl;
        return processes;
    }

    uint64_t totalCpuTime = getTotalCpuTime();

    dirent* entry = readdir(dir);

    while (entry != nullptr) {
        if (entry->d_type == DT_DIR) {
            char* end;
            int32_t pid = strtol(entry->d_name, &end, 10);
            if (*end == '\0') {
                processes.push_back(getProcessInfo(pid, totalCpuTime));
            }
        }
        entry = readdir(dir);
    }

    closedir(dir);

    return processes;
}

int main() {
    int previousLineCount = 0;

    while (true) {
        std::vector<Process> processes = getProccesses();

        // Clear the previously printed lines
        for (int i = 0; i < previousLineCount; ++i) {
            std::cout << "\033[A\033[K"; // Move up and clear line
        }

        // Print new process information
        for (auto& process : processes) {
            process.printResources();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Update the line count for the next iteration
        previousLineCount = processes.size();
    }

    return 0;
}