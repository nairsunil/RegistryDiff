#pragma once
#include "ThreadPoolMgr.h"
#include <list>

class CDumpEngine
{
private:
    bool            m_isInitialized;
    CThreadPoolMgr  m_thPoolMgr;
public:
    CDumpEngine();
    ~CDumpEngine();

    bool Initialize();

};

