#ifndef _SYSTEMINFO_H_
#define _SYSTEMINFO_H_

#include<cassert>

//ΩÃ±€≈Ê
class SystemInfo {
public:
    static SystemInfo& instance() {
        static SystemInfo* instance = new SystemInfo();
        return *instance;
    }

    SystemInfo() {
        agentAmount = 1;
        modelKindNum = 0;
    }

    ~SystemInfo() {
    }

    int agentAmount;
    int modelKindNum;
    int _deltaTime;

private:

    static SystemInfo* instance_;
};

//SystemInfo* SystemInfo::instance_ = nullptr;

#endif