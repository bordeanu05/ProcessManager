#ifndef PROCESS_H
#define PROCESS_H

#include <cstdint>
#include <iostream>

class Process {
private:
    int32_t m_pid;
    int32_t m_cpu;
    int32_t m_mem;

public:
    Process();
    Process(const int32_t& pid, const int32_t& cpu, const int32_t& mem);

    ~Process();

    int32_t getPid() const;
    int32_t getCpu() const;
    int32_t getMem() const;

    void setPid(const int32_t& pid);
    void setCpu(const int32_t& cpu);
    void setMem(const int32_t& mem);

    void printResources() const;
};

#endif // PROCESS_H