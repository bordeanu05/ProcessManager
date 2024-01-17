#include "../include/process.h"
#include <iostream>

namespace process_util {

Process::Process() : m_name("nan"), m_user("nan"), m_pid(0), m_mem(0), m_cpu(0) {}

Process::Process(const std::string& name, const std::string& user, const int32_t& pid, const int32_t& mem, const double& cpu) : m_name(name), m_user(user), m_pid(pid), m_mem(mem), m_cpu(cpu) {}

Process::~Process() {}

std::string Process::getName() const {
    return m_name;
}

std::string Process::getUser() const {
    return m_user;
}

int32_t Process::getPid() const {
    return m_pid;
}

int32_t Process::getMem() const {
    return m_mem;
}

double Process::getCpu() const {
    return m_cpu;
}

void Process::setName(const std::string& name) {
    m_name = name;
}

void Process::setUser(const std::string& user) {
    m_user = user;
}

void Process::setPid(const int32_t& pid) {
    m_pid = pid;
}

void Process::setMem(const int32_t& mem) {
    m_mem = mem;
}

void Process::setCpu(const double& cpu) {
    m_cpu = cpu;
}

} // namespace process_util