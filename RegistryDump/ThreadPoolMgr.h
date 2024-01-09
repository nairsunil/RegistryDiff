#pragma once
#include "..\\stdafx.h"
#include "CommonConstructs.h"
#include <Windows.h>


class CThreadPoolMgr
{
private:
    PTP_POOL m_pool;
    _TP_CALLBACK_ENVIRON_V3 m_environment;
    PTP_CLEANUP_GROUP m_cleanupGroup;
public:
    static volatile LONG nTaskCount;
    static HANDLE sHTaskComplete;
public:
    CThreadPoolMgr();
    ~CThreadPoolMgr();

    bool Initalize();
    void Cleanup();
    PTP_WORK AddTask(IThreadPoolTask *);
};

