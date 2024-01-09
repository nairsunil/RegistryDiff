#pragma once
#include "..\\stdafx.h"
#include "CommonConstructs.h"
#include "HiveFileSerializer.h"

class CLoadAndDumpHive : public IHiveSerializeAlgorithm
{
private:
    CHiveFileSerializer     *m_pFileSerialzer;
private:
    void LoadHiveInMemory(HKEY hRootKey, TCHAR* lpszSubKey, struct RegistryRecord** currentNode);
    void DumpHiveInFile(struct RegistryRecord* recList);
public:
    static CRITICAL_SECTION m_cs;
    CLoadAndDumpHive(CHiveFileSerializer* pFileSerializer);
    ~CLoadAndDumpHive();

    bool SerializeHive(CRegistryHive *);
};

