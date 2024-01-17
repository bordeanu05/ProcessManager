#ifndef PROCESS_H
#define PROCESS_H

#include <cstdint>
#include <string>

namespace process_util {
class Process {
private:
    std::string m_name;
    std::string m_user;
    int32_t m_pid;
    int32_t m_mem;
    double m_cpu;

public:
    Process();
    Process(const std::string& name, const std::string& user, const int32_t& pid, const int32_t& mem, const double& cpu);

    ~Process();

    std::string getName() const;
    std::string getUser() const;
    int32_t getPid() const;
    int32_t getMem() const;
    double getCpu() const;

    void setName(const std::string& name);
    void setUser(const std::string& user);
    void setPid(const int32_t& pid);
    void setMem(const int32_t& mem);
    void setCpu(const double& cpu);
};
} // namespace process_util

#endif // PROCESS_H