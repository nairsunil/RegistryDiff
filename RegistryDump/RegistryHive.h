#pragma once
#include "..\\stdafx.h"
#include "CommonConstructs.h"

class CRegistryHive : public IThreadPoolTask
{
private:
    HKEY            m_hRootKey;
    RegistryRecord  *m_pRegistryStartNode;

    IHiveSerializeAlgorithm *m_pSerializeAlgo;
public:
    CRegistryHive(HKEY hRootKey);
    ~CRegistryHive();

    HKEY GetRootKey() { return m_hRootKey; }
    void SetStartNode(RegistryRecord *pRecord) { m_pRegistryStartNode = pRecord; }
    void SetSerializeAlgorithm(IHiveSerializeAlgorithm *);
//private:
    bool SerializeHive();
public:
    bool ExecuteTask();
};

