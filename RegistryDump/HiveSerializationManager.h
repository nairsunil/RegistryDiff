#pragma once
#include "ThreadPoolMgr.h"
#include "RegistryHive.h"
#include "HiveFileSerializer.h"

#include <list>

using namespace std;

class CHiveSerializationMgr
{
private:
    static CHiveSerializationMgr    m_sInstance;
    CThreadPoolMgr                  *m_pTPExecutor;
    CHiveFileSerializer             *m_fileSerializer;
    list<CRegistryHive *>             m_HiveList;

    bool                            m_bIsInitialized;
private:
    CHiveSerializationMgr();
    ~CHiveSerializationMgr();

public:
    static CHiveSerializationMgr& instance() { return m_sInstance; };
    
    bool Initialize();
    void SetHiveForSerialization(HKEY hRootKey);
    void SerializeAll();
    void SerializeHive(HKEY hRootKey);
    void WaitForCompletion();
};

