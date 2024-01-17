#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <vector>
#include <iomanip>
#include <sstream>

#include "../include/process_util.h"

void printHeader(const double& totalCpuPercentage) {
    std::ostringstream cpuPercentage;
    cpuPercentage << "CPU(" << std::fixed << std::setprecision(2) << totalCpuPercentage << '%' << ')';

    std::ostringstream memUsage;
    memUsage << "Memory(" << std::fixed << std::setprecision(2) << process_util::getTotalMemUsage() << '%' << ')';

    std::cout << std::left << std::setw(10) << "PID" << std::setw(25) << "Name" << std::setw(25) << "User" << std::setw(25) << cpuPercentage.str() << std::setw(25) << memUsage.str() << std::endl;
    std::cout << std::string(110, '-') << std::endl;
}

void printProcessInfo(const process_util::Process& process) {
    std::ostringstream cpuPercentage;
    cpuPercentage << std::fixed << std::setprecision(2) << process.getCpu() << '%';

    std::ostringstream memPercentage;
    memPercentage << std::fixed << std::setprecision(2) << process.getMem() << "KB";

    std::cout << std::left << std::setw(10) << process.getPid() << std::setw(25) << process.getName() << std::setw(25) << process.getUser() << std::setw(25) << cpuPercentage.str() << std::setw(25) << memPercentage.str() << std::endl;
}

int main(int argc, char** argv) {
    double cpuPercentage = 0.0;

    while (true) {
        system("clear");

        std::vector<int64_t> startCPUTimes = process_util::getSystemCpuTimes();
        std::vector<process_util::Process> processes = process_util::getProccesses();

        printHeader(cpuPercentage);
        for (auto& process : processes) {
            printProcessInfo(process);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::vector<int64_t> endCPUTimes = process_util::getSystemCpuTimes();
        cpuPercentage = process_util::getSystemCpuPercentage(startCPUTimes, endCPUTimes);
    }

    return 0;
}