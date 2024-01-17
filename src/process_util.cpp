#include "../include/process.h"

#include <dirent.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <tuple>
#include <unistd.h>
#include <pwd.h>

namespace process_util {

int64_t getCpuHertz() {
    return sysconf(_SC_CLK_TCK);
}

int64_t getSystemUpTime() {
    std::ifstream uptimeFile("/proc/uptime");

    if (!uptimeFile.is_open()) {
        std::cerr << "Error: Could not open /proc/uptime" << std::endl;
        return 0;
    }

    std::string line;
    std::getline(uptimeFile, line);
    uptimeFile.close();

    std::istringstream iss(line);
    int64_t uptime;
    iss >> uptime;

    return uptime;
}

std::vector<int64_t> getSystemCpuTimes() {
    std::ifstream statFile("/proc/stat");

    if (!statFile.is_open()) {
        std::cerr << "Error: Could not open /proc/stat" << std::endl;
        return { 0 };
    }

    std::string line;
    std::getline(statFile, line);
    statFile.close();

    std::istringstream iss(line);

    std::string temp;
    iss >> temp;

    std::vector<int64_t> cpuTimes;
    int64_t time;
    while (iss >> time) {
        cpuTimes.push_back(time);
    }

    return cpuTimes;
}

double getSystemCpuPercentage(const std::vector<int64_t>& startCPUTimes, const std::vector<int64_t>& endCPUTimes) {
    unsigned long long idle_start = startCPUTimes[3];
    unsigned long long idle_end = endCPUTimes[3];

    unsigned long long total_start = 0;
    unsigned long long total_end = 0;
    for (int i = 0; i < startCPUTimes.size(); ++i) {
        total_start += startCPUTimes[i];
        total_end += endCPUTimes[i];
    }

    double idle = idle_end - idle_start;
    double total = total_end - total_start;
    return (1.0 - idle / total) * 100.0;
}

double getTotalMemUsage() {
    std::ifstream file("/proc/meminfo");
    std::string line;
    int64_t totalMemory = 0, freeMemory = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string label;
        int64_t value;
        iss >> label >> value;
        if (label == "MemTotal:") {
            totalMemory = value;
        }
        else if (label == "MemFree:") {
            freeMemory = value;
        }
    }

    int64_t usedMemory = totalMemory - freeMemory;
    return static_cast<double>(usedMemory) / totalMemory * 100.0;
}

// This is used to get the CPU time for a specific process
std::pair<int64_t, int64_t> getProcessCpuTimeAndProcessUpTime(const int32_t& pid) {
    std::ifstream statFile("/proc/" + std::to_string(pid) + "/stat");

    if (!statFile.is_open()) {
        std::cerr << "Error: Could not open /proc/" << pid << "/stat" << std::endl;
        return { 0, 0 };
    }

    std::string line;
    std::getline(statFile, line);
    statFile.close();

    std::istringstream iss(line);

    for (size_t i = 0; i < 13; ++i) {
        std::string temp;
        iss >> temp;
    }

    int64_t utime, stime, cutime, cstime, startTime;
    iss >> utime >> stime >> cutime >> cstime >> startTime;

    return std::make_pair(utime + stime + cutime + cstime, startTime);
}

double getProcessCpuPercentage(const int32_t& pid) {
    int64_t processCpuTime, processUpTime;
    std::tie(processCpuTime, processUpTime) = getProcessCpuTimeAndProcessUpTime(pid);

    int64_t systemUpTime = getSystemUpTime();

    int64_t seconds = systemUpTime - (processUpTime / getCpuHertz());

    if (seconds <= 0) {
        return 0.0;
    }

    return 100.0 * ((static_cast<double>(processCpuTime) / static_cast<double>(getCpuHertz())) / static_cast<double>(seconds));
}

// This is used to get the memory usage for a specific process
int64_t getProcessMem(const int32_t& pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/statm";

    std::ifstream statmFile(path);

    if (!statmFile.is_open()) {
        std::cerr << "Error: Could not open " << path << std::endl;
        return 0;
    }

    std::string line;
    std::getline(statmFile, line);
    statmFile.close();

    std::istringstream iss(line);

    int64_t mem;
    iss >> mem;

    return mem;
}

std::string getProcessName(const int32_t& pid) {
    std::ifstream commFile("/proc/" + std::to_string(pid) + "/comm");
    if (!commFile.is_open()) {
        std::cerr << "Error: Could not open /proc/" << pid << "/comm" << std::endl;
        return "nan";
    }

    std::string name;
    std::getline(commFile, name);
    commFile.close();

    return name;
}

std::string getProcessUser(const int32_t& pid) {
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
    if (!statusFile.is_open()) {
        std::cerr << "Error: Could not open /proc/" << pid << "/status" << std::endl;
        return "nan";
    }

    std::string line;
    std::string user;
    while (std::getline(statusFile, line)) {
        if (line.find("Uid:") != std::string::npos) {
            std::istringstream iss(line);
            std::string temp;
            iss >> temp;
            iss >> user;
            break;
        }
    }
    statusFile.close();

    std::ifstream passwdFile("/etc/passwd");
    if (!passwdFile.is_open()) {
        std::cerr << "Error: Could not open /etc/passwd" << std::endl;
        return "nan";
    }

    while (std::getline(passwdFile, line)) {
        if (line.find(user) != std::string::npos) {
            std::istringstream iss(line);
            std::string temp;
            iss >> temp;
            iss >> temp;
            iss >> temp;
            iss >> temp;
            iss >> temp;
            iss >> temp;
            iss >> temp;
            iss >> user;
            break;
        }
    }
    passwdFile.close();

    struct passwd* pwd = getpwuid(std::stoi(user));

    if (pwd == nullptr) {
        return "Unknown";
    }
    user = pwd->pw_name;

    return user;
}

// This is used to get the process information
Process getProcessInfo(const int32_t& pid) {
    Process process;
    std::string name = getProcessName(pid);
    std::string user = getProcessUser(pid);
    int32_t mem = static_cast<int32_t>(getProcessMem(pid));
    double cpu = (getProcessCpuPercentage(pid));

    return Process(name, user, pid, mem, cpu);
}

// This is used to get all the processes with their information
std::vector<Process> getProccesses() {
    std::vector<Process> processes;

    DIR* dir = opendir("/proc");
    if (dir == nullptr) {
        std::cerr << "Error: Could not open /proc" << std::endl;
        return processes;
    }

    dirent* entry = readdir(dir);

    while (entry != nullptr) {
        if (entry->d_type == DT_DIR) {
            char* end;
            int32_t pid = strtol(entry->d_name, &end, 10);
            if (*end == '\0') {
                processes.push_back(getProcessInfo(pid));
            }
        }
        entry = readdir(dir);
    }

    closedir(dir);

    return processes;
}

} // namespace util