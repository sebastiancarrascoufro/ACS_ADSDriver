#ifndef TestCache_H
#define TestCache_H

#include "Utils.h"
#include <TETimeUtil.h>
#include <thread>

class CacheAssemblyBase
{
public:
    CacheAssemblyBase(double duration);
    ~CacheAssemblyBase();
    void show();
    int getA();
    double getB();
protected:
    void getP(ACS::Time&);
private:
    int valueA;
    double valueB;
    ACS::Time refA;
    ACS::Time refB;
    ACS::Time refP;
    ACS::Time currentTime;
    //AMB::TimeThread* timeThread_p;
    std::thread t;
    AMB::Cache<CacheAssemblyBase, void> cacheParent;
};

void loop(ACS::Time& current)
{
    int i = 0;
    while(i < 100){
        ACE_VERSIONED_NAMESPACE_NAME::ACE_Time_Value t(ACE_OS::gettimeofday());
        current = (static_cast<ACS::Time>(t.sec())*10000000ULL +
                   static_cast<ACS::Time>(t.usec())*10ULL);
        i++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

CacheAssemblyBase::CacheAssemblyBase(double duration)
    : valueA(0),
      valueB(0.0),
      refA(0),
      refB(0),
      refP(0),
      currentTime(0),
      t(loop, std::ref(currentTime)),
      cacheParent(this, &CacheAssemblyBase::getP, duration, currentTime)
{
}

CacheAssemblyBase::~CacheAssemblyBase()
{
    t.join();
}

void CacheAssemblyBase::show()
{
    std::cout << valueA << " " << valueB << std::endl;
}

int CacheAssemblyBase::getA()
{
    cacheParent.getVal(refA);
    return valueA;
}

double CacheAssemblyBase::getB()
{
    cacheParent.getVal(refB);
    return valueB;
}

void CacheAssemblyBase::getP(ACS::Time& timestamp)
{
    valueA++;
    valueB+=1.2;
    refP = timestamp;
    refA = refP;
    refB = refP;
}

#endif
