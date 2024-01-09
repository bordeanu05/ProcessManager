#include "../include/process.h"

Process::Process() : m_pid(0), m_cpu(0), m_mem(0) {}

Process::Process(const int32_t& pid, const int32_t& cpu, const int32_t& mem) : m_pid(pid), m_cpu(cpu), m_mem(mem) {}

Process::~Process() {}

int32_t Process::getPid() const {
    return m_pid;
}

int32_t Process::getCpu() const {
    return m_cpu;
}

int32_t Process::getMem() const {
    return m_mem;
}

void Process::setPid(const int32_t& pid) {
    m_pid = pid;
}

void Process::setCpu(const int32_t& cpu) {
    m_cpu = cpu;
}

void Process::setMem(const int32_t& mem) {
    m_mem = mem;
}

void Process::printResources() const {
    std::cout << "PID: " << m_pid << " | " << "CPU: " << m_cpu << '%' << " | " << "MEM: " << m_mem << "KB" << std::endl;
}