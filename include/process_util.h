#ifndef PROCESS_UTIL_H
#define PROCESS_UTIL_H

#include <cstdint>
#include "process.h"

namespace process_util {

int64_t getTotalCpuTime();
int64_t getProcessCpuTime(const int32_t& pid);
int64_t getProcessMem(const int32_t& pid);
Process getProcessInfo(const int32_t& pid, const int64_t& totalCpuTime);
std::vector<Process> getProccesses();
double getTotalMemUsage();
std::vector<int64_t> getSystemCpuTimes();
double getSystemCpuPercentage(const std::vector<int64_t>& startCPUTimes, const std::vector<int64_t>& endCPUTimes);

} // namespace process_util

#endif // PROCESS_UTIL_H